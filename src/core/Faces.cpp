//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// Faces.cpp
//
// Written by: Kuan-Min Lee
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2026, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include<algorithm>
#include <math.h>
#include "Faces.hpp"
  
Faces::Faces(const int nV, const vector<int>& coordIndex) {
  // TODO
  // store a copy of coordIndex
  coordIndex_ = coordIndex;  
  
  // storage variables for:
  // 1. number of vertices
  // 2. number of faces
  // 3. number of corners
  int max_idx = -1; // for storing the maximum coordIndex (for calculating the number of vertices)
  num_faces_ = 0; // for storing the number of faces
  num_corners_ = (int)coordIndex.size(); // for storing the number of angles
  
  // storage variables for:
  // 1. current face-vertices coordinate index dictionary (face_id, vertex_id)
  // 2. current face-corneres coordinate index dictrionary (face_id, corner_id)
  std::vector<int> current_face_vertice_coordIndex; // for storing vertices indices for each face
  std::vector<int> current_face_corners_coordIndex; // for storing the angle indices for each face
  // clean the parameters before storing any value (just in case)
  face_vertice_coordIndex_.clear();
  face_corners_coordIndex_.clear();
  
  int c_idx = 0; // corner index
  for (int idx : coordIndex){
	  // non-negative index 
	  if (idx >= 0){
		// current index is larger than the stored vertices index, replace it
		// (for vertices number calculation)
		if (idx > max_idx){
			max_idx = idx;
		}
		// add this index to current face coordinate
		current_face_vertice_coordIndex.push_back(idx);
		// add this angle index to current face angle coordinate and update it
		current_face_corners_coordIndex.push_back(c_idx);
		
	  // the -1 case
	  } else {
	      if (!current_face_vertice_coordIndex.empty()){
			  // everytime we hits -1, we update the vertices and corners dictionary with current vertices and corners dictionary
			  face_vertice_coordIndex_.push_back(current_face_vertice_coordIndex);
			  face_corners_coordIndex_.push_back(current_face_corners_coordIndex);
			  // everytime we hits -1, we add one to the number of faces
			  num_faces_ += 1;
		  }
		  // clear current face vertice and corner coordIndex
		  current_face_vertice_coordIndex.clear();
		  current_face_corners_coordIndex.clear();
	  }
	  
	  // update corner index
	  c_idx += 1;
  }
  
  // push the unprocessed coordIndex (just in case)
  if (!current_face_vertice_coordIndex.empty()){
	  face_vertice_coordIndex_.push_back(current_face_vertice_coordIndex);
	  face_corners_coordIndex_.push_back(current_face_corners_coordIndex);
	  // everytime we hits -1, we add one to the number of faces
	  num_faces_ += 1;
  }
  // clear current face vertice and corner coordIndex
  current_face_vertice_coordIndex.clear();
  current_face_corners_coordIndex.clear();
  
  // store necessary variables
  num_vertices_ = std::max(nV, (max_idx+1));  
  
}

int Faces::getNumberOfVertices() const {
  // TODO
  return num_vertices_;
}

int Faces::getNumberOfFaces() const {
  // TODO
  if (num_faces_ != 0){
	  return num_faces_;
  } else {
	return 0;
  }
}

int Faces::getNumberOfCorners() const {
  // TODO
  return num_corners_;
}

int Faces::getFaceSize(const int iF) const {
  // TODO
  // only return when the face index is valid
  if (iF >= 0 && iF < num_faces_){
	 return (int)face_vertice_coordIndex_[iF].size(); 
  // if invalid, just return 0
  }
  return 0;
}

int Faces::getFaceFirstCorner(const int iF) const {
  // TODO
  // return the first corner index only if the face id is valid
  if (iF >= 0 && iF < num_faces_ && !face_corners_coordIndex_[iF].empty()){
	 return face_corners_coordIndex_[iF][0]; 
  }
  return -1;
}

int Faces::getFaceVertex(const int iF, const int j) const {
  // TODO
  // return the corner only if the face and corner id are valid
  
  //setup variable for storage (the most painful part as a Python guy)
  std::vector<int> current_face_corners_coordIndex; // for storing the angle indices for each face
  int current_face_number_corners; // for storing the number of corners for current face

  // only return outcome when the face index and j index are valid
  if (iF >= 0 && iF < num_faces_){
	  current_face_corners_coordIndex = face_corners_coordIndex_[iF];
	  current_face_number_corners = current_face_corners_coordIndex.size();
	  if (j >= 0 && j < current_face_number_corners){
	  // if (std::find(current_face_corners_coordIndex.begin(), current_face_corners_coordIndex.end(), j) != current_face_corners_coordIndex.end()){
		  return face_vertice_coordIndex_[iF][j];
	  }
  }
  // return -1 otherwise
  return -1;
}

int Faces::getCornerFace(const int iC) const {
  // TODO
  // check if the current iC is out of range
  if (iC < 0 || iC >= num_corners_){
	  return -1;
  }
  // check if the current coordinate is -1
  if (coordIndex_[iC] == -1){
	  return -1;
  }
  
  //setup variable for storage
  std::vector<int> current_face_corners_coordIndex; // for storing the angle indices for each face
  int current_face_number_corners; // for storing the number of corners for current face
  int current_corner_id; // for storing the current looped corner id
  
  // if valid, check for corner coordinate index dictionary we previously processed
  if (iC >= 0 && iC < num_corners_){
	  for (int idx_face = 0; idx_face < num_faces_; idx_face++){
		  current_face_corners_coordIndex = face_corners_coordIndex_[idx_face];
		  current_face_number_corners = current_face_corners_coordIndex.size();
		  for (int idx_corner = 0; idx_corner < current_face_number_corners; idx_corner++){
			  current_corner_id = current_face_corners_coordIndex[idx_corner];
			  // if the corner id is found, return it
			  if (current_corner_id == iC){
				  return idx_face;
			  }
		  }
	  }
  } 
  return -1;
}

int Faces::getNextCorner(const int iC) const {
  // TODO
  // check if iC is valid
  if (iC < 0 || iC >= num_corners_){
	  return -1;
  }
  // if the current iC is -1
  if (coordIndex_[iC] == -1){
	  return -1;
  }
  
  //setup variable for storage
  std::vector<int> current_face_corners_coordIndex; // for storing the angle indices for each face
  int current_face_number_corners; // for storing the number of corners for current face
  int current_corner_id; // for storing the current looped corner id
  
  // if valid, check for corner coordinate index dictionary we previously processed
  if (iC >= 0 && iC < num_corners_){
	  for (int idx_face = 0; idx_face < num_faces_; idx_face++){
		  current_face_corners_coordIndex = face_corners_coordIndex_[idx_face];
		  current_face_number_corners = current_face_corners_coordIndex.size();
		  for (int idx_corner = 0; idx_corner < current_face_number_corners; idx_corner++){
			  current_corner_id = current_face_corners_coordIndex[idx_corner];
			  // if the corner id is found, check for its next corner
			  if (current_corner_id == iC){
				  // if this is the last angle, cycle back clockwise
				  if (idx_corner == current_face_number_corners - 1){
					  return current_face_corners_coordIndex[0];
				  // other than that, return the next angle
				  } else {
					return current_face_corners_coordIndex[idx_corner+1];
				  }
			  }
		  }
		  
	  }
  } 
  return -1;
}


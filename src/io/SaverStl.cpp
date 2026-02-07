//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// SaverStl.cpp
//
// Written by: <Your Name>
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

#include "SaverStl.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

#include "core/Faces.hpp"

const char* SaverStl::_ext = "stl";

//////////////////////////////////////////////////////////////////////
void SaverStl::saveFace
(FILE* fp, const Faces& faces, int face_id, vector<float>& coord, vector<float>& Normal) const {
  // if the normal vector is empty, just return the function
  if(Normal.size() == 0) return;
  
  // Normap Vector Part //
  // write normal vector
  fprintf(fp, " facet normal %f %f %f\n", Normal[3*face_id], Normal[3*face_id+1], Normal[3*face_id+2]);

  // Vertices Part //
  // write vertices
  fprintf(fp, "  outer loop\n");
  // loop through each vertices
  int number_vertices = faces.getFaceSize(face_id);
  if (number_vertices < 3) return;
  
  for (int i_vertices = 0; i_vertices < number_vertices; i_vertices++){
	  // retrieve current vertex index
	  int current_coordIndex = faces.getFaceVertex(face_id, i_vertices);
	  // write the vertex
	  fprintf(fp, "   vertex %f %f %f\n", coord[3*current_coordIndex], coord[3*current_coordIndex+1], coord[3*current_coordIndex+2]);	  
  }
  // write vertices end 
  fprintf(fp, "  endloop\n");
  // End of Vertices //

  // write normal vector end
  fprintf(fp, "endfacet\n");
  // End of Normal Vector //
  
}




//////////////////////////////////////////////////////////////////////
bool SaverStl::save(const char* filename, SceneGraph& wrl) const {
  bool success = false;
  if(filename!=(char*)0) {
    // Check these conditions

    // 1) the SceneGraph should have a single child
    // 2) the child should be a Shape node
    // 3) the geometry of the Shape node should be an IndexedFaceSet node

    // - construct an instance of the Faces class from the IndexedFaceSet
    // - remember to delete it when you are done with it (if necessary)
    //   before returning

    // 4) the IndexedFaceSet should be a triangle mesh
    // 5) the IndexedFaceSet should have normals per face

    // if (all the conditions are satisfied) {

    FILE* fp = fopen(filename,"w");
    if(fp!=(FILE*)0) {
		
      // if set, use ifs->getName()
      // otherwise use filename,
      // but first remove directory and extension

      fprintf(fp,"solid %s\n",filename);

      // TODO ...

	  // checking
	  // wrl: SceneGraph Object (Group-Node based)
		
	  // retrieve the number of children and check if the number of children is not one
	  // 1) check if SceneGraph only has one child
	  int num_children = wrl.getNumberOfChildren();
	  if (num_children != 1){
		return false;
	  }
	  // create a node pointer to the child node from SceneGraph
	  Node* child_node = wrl[0];
	  // 2) check if the child is Shape
	  // only process if it's a shape
	  if (!child_node->isShape()){
		return false;
	  }
	  // after checking the type cast it into shape
	  Shape* shape_child_node = (Shape*) child_node;
	  
	  // 3) check if the child has IndexedFaceSet class Geometry
	  if (!shape_child_node->hasGeometryIndexedFaceSet()){
		return false;
	  }	  
		
      // retrieve geometry (IndexedFaceSet)
	  Node* shape_child_node_geometry = shape_child_node->getGeometry();
	  IndexedFaceSet* shape_child_node_IndexFaceSet = (IndexedFaceSet*) shape_child_node_geometry;

	  // Check geometriy Face Index Set
	  // check if the FaceSet if Triangle Mesh
	  // if (!shape_child_node_IndexFaceSet->isTriangleMesh()) return false;
	  // check if the FaceSet is empty in normal vector
	  // if (shape_child_node_IndexFaceSet->getNormal().empty()) return false;

	  // necessary items
	  std::vector<float>& coord = shape_child_node_IndexFaceSet->getCoord(); // Vertices Coordinate
	  std::vector<int>& coordIndex = shape_child_node_IndexFaceSet->getCoordIndex(); // CoordIndex
	  std::vector<float>& normal_vector = shape_child_node_IndexFaceSet->getNormal(); // Normal vector (maybe?)

	  // set up Face object
	  int nV = coord.size()/3;
	  Faces child_node_faces(nV, coordIndex);
	  	  
	  // loop through each face
	  int num_faces_child_node = child_node_faces.getNumberOfFaces();
	  for (int i_face = 0; i_face < num_faces_child_node; i_face++) {
		  saveFace(fp, child_node_faces, i_face, coord, normal_vector);
	  }
	  
	  
      // for each face {
      //   ...
      // }
      
	  fprintf(fp, "endsolid %s\n", filename);
	  
      fclose(fp);
      success = true;
    }
    // } endif (all the conditions are satisfied)

  }
  return success;
}

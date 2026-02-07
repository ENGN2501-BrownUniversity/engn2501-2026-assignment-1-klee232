//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// LoaderStl.cpp
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

#include <stdio.h>
#include "TokenizerFile.hpp"
#include "LoaderStl.hpp"
#include "StrException.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

// reference
// https://en.wikipedia.org/wiki/STL_(file_format)

const char* LoaderStl::_ext = "stl";

bool LoaderStl::loadFace(Tokenizer&tkn, std::vector<float>& normal, std::vector<float>& coord, std::vector<int>& coordIndex){
	// double check
	if(!tkn.equals("facet")) throw new StrException("Expected 'facet'");
	
	// normal vector part
	if (tkn.expecting("normal")){
		float norm_coordinate;
		int norm_coord_num = 0;
		while(norm_coord_num < 3){
			if (!tkn.getFloat(norm_coordinate)) throw new StrException("Expected float");
			normal.push_back(norm_coordinate);
			norm_coord_num += 1;
		}
	} else {
		throw new StrException("Expected 'normal'");
	}
	
	// coordinate part
	int base = coord.size()/3;
	if (!tkn.expecting("outer") || !tkn.expecting("loop")) throw new StrException("Expected 'outer loop'");
	
	for (int i_line=0; i_line < 3; i_line++){
		if (tkn.expecting("vertex")) {
			float coordinate;
			int coord_num = 0;
			while(coord_num < 3){
				if (tkn.getFloat(coordinate)){
					coord.push_back(coordinate);
					coord_num += 1;
				} else {
					throw new StrException("Expected float");
				}
			}
			coordIndex.push_back(base + i_line);
		} else {
			throw new StrException("Expected 'vertex'");
		}
	}
	coordIndex.push_back(-1);
	
	// end loop
	if (!tkn.expecting("endloop")) throw new StrException("Expected 'endloop'");
	
	// end facet
    if (!tkn.expecting("endfacet")) throw new StrException("Expected 'endfacet'");

	return true;
}

bool LoaderStl::load(const char* filename, SceneGraph& wrl) {
  bool success = false;

  // clear the scene graph
  wrl.clear();
  wrl.setUrl("");

  FILE* fp = (FILE*)0;
  try {

    // open the file
    if(filename==(char*)0) throw new StrException("filename==null");
    fp = fopen(filename,"r");
    if(fp==(FILE*)0) throw new StrException("fp==(FILE*)0");

    // use the io/Tokenizer class to parse the input ascii file
    TokenizerFile tkn(fp);
	
    // first token should be "solid"
    if(tkn.expecting("solid") && tkn.get()) {
      string stlName = tkn; // second token should be the solid name

      // TODO ...

      // create the scene graph structure :
      // 1) the SceneGraph should have a single Shape node a child
      // 2) the Shape node should have an Appearance node in its appearance field
      // 3) the Appearance node should have a Material node in its material field
      // 4) the Shape node should have an IndexedFaceSet node in its geometry node
      // from the IndexedFaceSet
      // 5) get references to the coordIndex, coord, and normal arrays
      // 6) set the normalPerVertex variable to false (i.e., normals per face)  

      // Create a shape children node for scene graph
	  // whole shape
	  Shape* child_node = new Shape();
	  // Appearance node
	  Appearance* child_node_appearance = new Appearance();
	  Material* child_node_appearance_material = new Material();
	  child_node_appearance->setMaterial(child_node_appearance_material);
	  // IndexedFaceSet node
	  IndexedFaceSet* child_node_geometry = new IndexedFaceSet();
	  // store them back to shape
	  child_node->setAppearance(child_node_appearance);
	  child_node->setGeometry(child_node_geometry);
	  
	  // assign them to scene graph
	  wrl.addChild(child_node);

      // get references to coordIndex, coord, normal arrays
	  std::vector<int>& child_node_coordIndex = child_node_geometry->getCoordIndex();
	  std::vector<float>& child_node_coord = child_node_geometry->getCoord();
	  std::vector<float>& child_node_normal = child_node_geometry->getNormal();
	  
	  // set the normalPerVertex as false
	  child_node_geometry->setNormalPerVertex(false);
	  
	  // parsing the file
	  if (!tkn.get()) throw new StrException("Unexpected EOF after 'solid <name>'");
	  while (true){
		  if (tkn.equals("facet")){
			loadFace(tkn, child_node_normal, child_node_coord, child_node_coordIndex);
			if (!tkn.get()){
				success = true;
				break;
			}
		  } else if(tkn.equals("endsolid")){
			  success = true;
			  break;
		  } else {
			  throw new StrException("Expected 'facet' or 'endsolid'");
		  }
	  }

      // the file should contain a list of triangles in the following format

      // facet normal ni nj nk
      //   outer loop
      //     vertex v1x v1y v1z
      //     vertex v2x v2y v2z
      //     vertex v3x v3y v3z
      //   endloop
      // endfacet

      // - run an infinite loop to parse all the faces
      // - write a private method to parse each face within the loop
      // - the method should return true if successful, and false if not
      // - if your method returns tru
      //     update the normal, coord, and coordIndex variables
      // - if your method returns false
      //     throw an StrException explaining why the method failed

    }

    // close the file (this statement may not be reached)
    fclose(fp);
    
  } catch(StrException* e) { 
    
    if(fp!=(FILE*)0) fclose(fp);
    fprintf(stderr,"ERROR | %s\n",e->what());
    delete e;

  }

  return success;
}


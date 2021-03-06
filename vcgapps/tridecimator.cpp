//
// structured - Tools for the Generation and Visualization of Large-scale
// Three-dimensional Reconstructions from Image Data. This software includes
// source code from other projects, which is subject to different licensing,
// see COPYING for details. If this project is used for research see COPYING
// for making the appropriate citations.
// Copyright (C) 2013 Matthew Johnson-Roberson <mattkjr@gmail.com>
//
// This file is part of structured.
//
// structured is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// structured is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with structured.  If not, see <http://www.gnu.org/licenses/>.
//
/****************************************************************************
 * VCGLib                                                            o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004                                                \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
 * All rights reserved.                                                      *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
 * for more details.                                                         *
 *                                                                           *
 ****************************************************************************/
#include <vector>
#include <limits>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

// stuff to define the mesh
#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/edge/base.h>


#include <vcg/math/quadric.h>
#include <vcg/complex/algorithms/clean.h>

// io
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/export_obj.h>

// *include the algorithms for updating: */
#include <vcg/complex/algorithms/update/topology.h>	/* topology */
#include <vcg/complex/algorithms/update/bounding.h>	/* bounding box */
#include <vcg/complex/algorithms/update/normal.h>		/* normal */
#include<vcg/complex/append.h>

// local optimization
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>

using namespace vcg;
using namespace tri;
#include "meshmodel.h"
/**********************************************************
Mesh Classes for Quadric Edge collapse based simplification

For edge collpases we need verteses with:
- V->F adjacency
- per vertex incremental mark
- per vertex Normal


Moreover for using a quadric based collapse the vertex class 
must have also a Quadric member Q();
Otherwise the user have to provide an helper function object 
to recover the quadric.

******************************************************/
// The class prototypes.
/*class MyVertex;
class MyEdge;    
class MyFace;

struct MyUsedTypes: public UsedTypes<Use<MyVertex>::AsVertexType,Use<MyEdge>::AsEdgeType,Use<MyFace>::AsFaceType>{};

class MyVertex  : public Vertex< MyUsedTypes,
  vertex::VFAdj, 
  vertex::Coord3f, 
  vertex::Color4b,
  vertex::Normal3f, 
  vertex::Mark, 
  vertex::BitFlags  >{
public: 
  vcg::math::Quadric<double> &Qd() {return q;}
private:
  math::Quadric<double> q;
  };

class DummyType; 
class MyEdge : public Edge<MyUsedTypes,edge::VertexRef> {
public:
  inline MyEdge() {};
  inline MyEdge( MyVertex * v0, MyVertex * v1){V(0) = v0; V(1) = v1; };
	  static inline MyEdge OrderedEdge(MyVertex* v0,MyVertex* v1){
   if(v0<v1) return MyEdge(v0,v1);
   else return MyEdge(v1,v0);
  }

//  inline MyEdge( Edge<MyEdge,MyVertex> &e):Edge<MyEdge,MyVertex>(e){};
};


class MyFace    : public Face< MyUsedTypes,
  face::VFAdj, 
  face::VertexRef, 
  face::BitFlags > {};

/// the main mesh class 
class MyMesh    : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace> > {};


class MyTriEdgeCollapse: public vcg::tri::TriEdgeCollapseQuadric< MyMesh, MyTriEdgeCollapse, QInfoStandard<MyVertex>  > {
						public:
						typedef  vcg::tri::TriEdgeCollapseQuadric< MyMesh,  MyTriEdgeCollapse, QInfoStandard<MyVertex>  > TECQ;
            typedef  MyMesh::VertexType::EdgeType EdgeType;
            inline MyTriEdgeCollapse(  const EdgeType &p, int i) :TECQ(p,i){}
};
*/

class MyTriEdgeCollapse: public vcg::tri::TriEdgeCollapseQuadric< CMeshO, MyTriEdgeCollapse, QInfoStandard<CVertexO>  > {
                                                public:
                                                typedef  vcg::tri::TriEdgeCollapseQuadric< CMeshO,  MyTriEdgeCollapse, QInfoStandard<CVertexO>  > TECQ;
            typedef  CVertexO::VertexType::EdgeType EdgeType;
            inline MyTriEdgeCollapse(  const EdgeType &p, int i) :TECQ(p,i){}
};
#define MyMesh CMeshO
void Usage()
{
  	printf(
           "---------------------------------\n"
           "         TriSimp V.1.0 \n"
           "     http://vcg.isti.cnr.it\n"
           "    http://vcg.sourceforge.net\n"
           "   release date: "__DATE__"\n"
           "---------------------------------\n\n"
		  "TriDecimator 1.0 \n"__DATE__"\n"
			"Copyright 2003-2006 Visual Computing Lab I.S.T.I. C.N.R.\n"
      "\nUsage:  "\
      "tridecimator file1 file2 face_num [opt]\n"\
      "Where opt can be:\n"\
      "     -e# QuadricError threshold  (range [0,inf) default inf)\n"
			"     -b# Boundary Weight (default .5)\n"
			"     -q# Quality threshold (range [0.0, 0.866],  default .3 )\n"
			"     -n# Normal threshold  (degree range [0,180] default 90)\n"
			"     -E# Minimal admitted quadric value (default 1e-15, must be >0)\n"
			"     -Q[y|n]  Use or not Quality Threshold (default yes)\n"
			"     -N[y|n]  Use or not Normal Threshold (default no)\n"
			"     -A[y|n]  Use or not Area Weighted Quadrics (default yes)\n"
			"     -O[y|n]  Use or not vertex optimal placement (default yes)\n"
			"     -S[y|n]  Use or not Scale Independent quadric measure(default yes) \n"
			"     -B[y|n]  Preserve or not mesh boundary (default no)\n"
			"     -T[y|n]  Preserve or not Topology (default no)\n"
			"     -H[y|n]  Use or not Safe Heap Update (default no)\n"
		  "     -P       Before simplification, remove duplicate & unreferenced vertices\n"
                                       );
  exit(-1);
}

// mesh to simplify
MyMesh mesh;

int main(int argc ,char**argv){
if(argc<4) Usage();


	//int t0=clock();	

  int err=vcg::tri::io::Importer<MyMesh>::Open(mesh,argv[1]);

  if(err) 
  {
    printf("Unable to open mesh %s : '%s'\n",argv[1],vcg::tri::io::Importer<MyMesh>::ErrorMsg(err));
    exit(-1);
  }
	printf("mesh loaded %d %d \n",mesh.vn,mesh.fn);
    
  TriEdgeCollapseQuadricParameter &qparams = MyTriEdgeCollapse::Params() ;
  MyTriEdgeCollapse::SetDefaultParams();
  qparams.QualityThr  =.3;
  bool FlipFlag=false;
  float TargetError=numeric_limits<float>::max();
  bool CleaningFlag =false;
  bool UseVertStop=false;
  int splitting=0;
  char basename[1024];
  strcpy(basename,"ipad");
     // parse command line.
	  for(int i=4; i < argc;)
    {
      if(argv[i][0]=='-')
        switch(argv[i][1])
      { 
        case 'H' : MyTriEdgeCollapse::Params().SafeHeapUpdate=true; printf("Using Safe heap option\n"); break;
        case 'Q' : if(argv[i][2]=='y') { qparams.QualityCheck	= true;  printf("Using Quality Checking\n");	}
                                  else { qparams.QualityCheck	= false; printf("NOT Using Quality Checking\n");	}                break;		
				case 'N' : if(argv[i][2]=='y') { qparams.NormalCheck	= true;  printf("Using Normal Deviation Checking\n");	}
                                  else { qparams.NormalCheck	= false; printf("NOT Using Normal Deviation Checking\n");	}        break;		
				case 'O' : if(argv[i][2]=='y') { qparams.OptimalPlacement	= true;  printf("Using OptimalPlacement\n");	}
                                  else { qparams.OptimalPlacement	= false; printf("NOT Using OptimalPlacement\n");	}        break;		
				case 'S' : if(argv[i][2]=='y') { qparams.ScaleIndependent	= true;  printf("Using ScaleIndependent\n");	}
                                  else { qparams.ScaleIndependent	= false; printf("NOT Using ScaleIndependent\n");	}        break;		
				case 'B' : if(argv[i][2]=='y') { qparams.PreserveBoundary	= true;  printf("Preserving Boundary\n");	}
                                  else { qparams.PreserveBoundary	= false; printf("NOT Preserving Boundary\n");	}        break;		
				case 'T' : if(argv[i][2]=='y') { qparams.PreserveTopology	= true;  printf("Preserving Topology\n");	}
                                  else { qparams.PreserveTopology	= false; printf("NOT Preserving Topology\n");	}        break;		
				case 'q' :	qparams.QualityThr	= atof(argv[i]+2);	           printf("Setting Quality Thr to %f\n",atof(argv[i]+2)); 	 break;			
				case 'n' :	qparams.NormalThrRad = math::ToRad(atof(argv[i]+2));  printf("Setting Normal Thr to %f deg\n",atof(argv[i]+2)); break;	
				case 'b' :	qparams.BoundaryWeight  = atof(argv[i]+2);			printf("Setting Boundary Weight to %f\n",atof(argv[i]+2)); break;		
				case 'e' :	TargetError = float(atof(argv[i]+2));			printf("Setting TargetError to %g\n",atof(argv[i]+2)); break;		
				case 'P' :	CleaningFlag=true;  printf("Cleaning mesh before simplification\n"); break;	
                                case 'F' : FlipFlag=true; break;
                                case 'V': UseVertStop=true;break;
                                case 's' :	splitting = atoi(argv[i]+2);  printf("Splitting output at %d\n",atoi(argv[i]+2)); break;
                                case 'u' :	strcpy(basename ,argv[i]+2);  printf("Splitting output fn %s\n",basename); break;

				default  :  printf("Unknown option '%s'\n", argv[i]);
          exit(0);
      }
      i++;
    }



  if(CleaningFlag){
      int dup = tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);
      int dup2 = tri::Clean<MyMesh>::RemoveDuplicateFace(mesh);
      int unref =  tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
      printf("Removed %i duplicate vetex %i duplicate faces  and %i unreferenced vertices from mesh \n",dup,dup2,unref);

  }
  if(FlipFlag)
      tri::Clean<MyMesh>::FlipMesh(mesh);

  printf("mesh loaded %d %d \n",mesh.vn,mesh.fn);

  int FinalSize=atoi(argv[3]);
if(FinalSize ==0 )
    FinalSize=mesh.fn;
  printf("reducing it to %i\n",FinalSize);
	
	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
  
	// decimator initialization
  vcg::LocalOptimization<MyMesh> DeciSession(mesh);
	
	int t1=clock();		
	DeciSession.Init<MyTriEdgeCollapse >();
  int t2=clock();	
  printf("Initial Heap Size %i\n",DeciSession.h.size());

  DeciSession.SetTimeBudget(0.5f);
  if(TargetError< numeric_limits<float>::max() ) DeciSession.SetTargetMetric(TargetError);

  if(UseVertStop){
      DeciSession.SetTargetVertices(FinalSize);
      printf("Using Vertex Stop\n");
      while(DeciSession.DoOptimization() && mesh.vn>FinalSize && DeciSession.currMetric < TargetError)
        printf("Current Mesh size %7i heap sz %9i err %9g \r",mesh.fn,DeciSession.h.size(),DeciSession.currMetric);
  }
  else{
      DeciSession.SetTargetSimplices(FinalSize);
      while(DeciSession.DoOptimization() && mesh.fn>FinalSize && DeciSession.currMetric < TargetError)
        printf("Current Mesh size %7i heap sz %9i err %9g \r",mesh.fn,DeciSession.h.size(),DeciSession.currMetric);
}

  int t3=clock();	
  printf("mesh  %d %d Error %g \n",mesh.vn,mesh.fn,DeciSession.currMetric);
  printf("\nCompleted in (%i+%i) msec\n",t2-t1,t3-t2);
  vcg::tri::io::PlyInfo pi;
  pi.mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
  pi.mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
  pi.mask |= vcg::tri::io::Mask::IOM_FACEQUALITY;

  vcg::tri::io::ExporterPLY<MyMesh>::Save(mesh,argv[2],true,pi);
  if(splitting >0){
      char fname[1024];
      int mesh_count=0;
      int vertexLeft=mesh.vn;
      while(vertexLeft>0){
          CMeshO destMesh;

          int selectedV=0;
          CMeshO::VertexIterator vi;
          CMeshO::FaceIterator   fi;
          for(vi=mesh.vert.begin();vi!=mesh.vert.end() && selectedV<splitting;++vi){
              if(!(*vi).IsD() && !(*vi).IsS() ){
                  (*vi).SetS();
                  selectedV++;
              }
          }



          // select all points involved
        //  tri::UpdateSelection<CMeshO>::ClearVertex(mesh);
          tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(mesh);
           tri::UpdateSelection<CMeshO>::ClearVertex(mesh);
           tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(mesh);

          tri::Append<CMeshO,CMeshO>::Mesh(destMesh, mesh, true);

          int numFacesSel = tri::UpdateSelection<CMeshO>::CountFace(mesh);
          int numVertSel  = tri::UpdateSelection<CMeshO>::CountVertex(mesh);

          printf("Moving %d\n",numFacesSel);
          tri::UpdateSelection<CMeshO>::ClearVertex(mesh);
          tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(mesh);
          for(fi=mesh.face.begin();fi!=mesh.face.end();++fi)
              if(!(*fi).IsD() && (*fi).IsS() )
                  tri::Allocator<CMeshO>::DeleteFace(mesh,*fi);
          for(vi=mesh.vert.begin();vi!=mesh.vert.end();++vi)
              if(!(*vi).IsD() && (*vi).IsS() )
                  tri::Allocator<CMeshO>::DeleteVertex(mesh,*vi);
          vcg::tri::Allocator<CMeshO>::CompactFaceVector(mesh);

          tri::UpdateSelection<CMeshO>::ClearVertex(mesh);
          tri::UpdateSelection<CMeshO>::ClearFace(mesh);

          sprintf(fname,"%s-%04d.obj",basename,mesh_count++);
          int mask = pi.mask;
          destMesh.textures.resize(0);
          //mask &= ~vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
          mask &= vcg::tri::io::Mask::IOM_VERTNORMAL;

          vcg::tri::UpdateFlags<CMeshO>::VertexClear(destMesh,CMeshO::VertexType::SELECTED);
          vcg::tri::UpdateFlags<CMeshO>::FaceClear(destMesh,CMeshO::FaceType::SELECTED);
          tri::UpdateBounding<CMeshO>::Box(destMesh);						// updates bounding box
                               for(fi=destMesh.face.begin();fi!=destMesh.face.end();++fi)	// face normals
                                       face::ComputeNormalizedNormal(*fi);
                               tri::UpdateNormals<CMeshO>::PerVertex(destMesh);				// vertex normals
          vcg::tri::io::ExporterOBJ<CMeshO>::Save(destMesh,fname, mask);
          printf("Writing %s verts: %d faces : %d %s\n",fname,destMesh.vn,destMesh.fn,fname);
          vertexLeft=0;
          for(vi=mesh.vert.begin();vi!=mesh.vert.end();++vi){
              if(!(*vi).IsD())
                  vertexLeft++;
          }
          printf("Vert left %d\n",vertexLeft);
          //  currentMesh->clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
          vcg::tri::UpdateFlags<CMeshO>::VertexClear(mesh,CMeshO::VertexType::SELECTED);
                vcg::tri::UpdateFlags<CMeshO>::FaceClear(mesh,CMeshO::FaceType::SELECTED);
       //   vcg::tri::UpdateFlags<CMeshO>::FaceClear(destMesh,CMeshO::FaceType::SELECTED);
          //Log("Moved %i faces and %i vertices to layer %i", numFacesSel, numVertSel, md.meshList.size());
      }
      sprintf(fname,"%s.txt");

      FILE *fp=fopen(fname,"w");
      fprintf(fp,"%d\n",mesh_count-1);
      fclose(fp);
  }
  return 0;

}

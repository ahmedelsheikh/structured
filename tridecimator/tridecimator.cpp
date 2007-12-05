#include <vector>
#include <limits>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

// stuff to define the mesh
#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/faceplus/base.h>
#include <vcg/simplex/edge/edge.h>
#include <vcg/complex/trimesh/base.h>

#include <vcg/math/quadric.h>
#include <vcg/complex/trimesh/clean.h>
#include<vcg/complex/trimesh/base.h>
#include<vcg/simplex/vertexplus/component_ocf.h>
#include<vcg/simplex/faceplus/component_ocf.h>
#include <vcg/complex/trimesh/update/topology.h>

// io
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export_ply.h>


// update
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>

// local optimization
#include <vcg/complex/local_optimization.h>
#include <vcg/complex/local_optimization/tri_edge_collapse_quadric.h>

using namespace vcg;
using namespace tri;
class CEdge;   
class CFaceO;
class CVertexO;

//Vert Mem Occupancy  --- 44b ---

class CVertexO  : public VertexSimp2< CVertexO, CEdge, CFaceO, 
  vert::Coord3f,     /* 12b */ 
  vert::BitFlags,    /*  4b */
  vert::Normal3f,    /* 12b */
  vert::Qualityf,    /*  4b */
  vert::VFAdj,       /*  4b */
  vert::Mark,        /*  4b */
  vert::Color4b      /*  4b */
  >{ 
};

  
class CEdge : public Edge<CEdge,CVertexO> {
public:
  inline CEdge() {};
  inline CEdge( CVertexO * v0, CVertexO * v1):Edge<CEdge,CVertexO>(v0,v1){};
  static inline CEdge OrderedEdge(VertexType* v0,VertexType* v1){
   if(v0<v1) return CEdge(v0,v1);
   else return CEdge(v1,v0);
  }

  //inline CEdge( Edge<CEdge,CVertexO> &e):Edge<CEdge,CVertexO>(e){};
};

//Face Mem Occupancy  --- 32b ---

class CFaceO    : public FaceSimp2<  CVertexO, CEdge, CFaceO,  
      face::InfoOcf,              /* 4b */
      face::VertexRef,            /*12b */
      face::BitFlags,             /* 4b */
      face::Normal3f,             /*12b */
      face::MarkOcf,              /* 0b */
      face::Color4bOcf,           /* 0b */
      face::FFAdjOcf,             /* 0b */
      face::VFAdjOcf,             /* 0b */
      face::WedgeTexturefOcf      /* 0b */
    > {};

class CMeshO    : public vcg::tri::TriMesh< vector<CVertexO>, face::vector_ocf<CFaceO> > {
public :
	int sfn; //The number of selected faces.
};
typedef	SimpleTempData<CMeshO::VertContainer, math::Quadric<double> > QuadricTemp;


class QHelper
		{
		public:
			QHelper(){};
      static void Init(){};
      static math::Quadric<double> &Qd(CVertexO &v) {return TD()[v];}
      static math::Quadric<double> &Qd(CVertexO *v) {return TD()[*v];}
      static CVertexO::ScalarType W(CVertexO * /*v*/) {return 1.0;};
      static CVertexO::ScalarType W(CVertexO & /*v*/) {return 1.0;};
      static void Merge(CVertexO & /*v_dest*/, CVertexO const & /*v_del*/){};
      static QuadricTemp* &TDp() {static QuadricTemp *td; return td;}
      static QuadricTemp &TD() {return *TDp();}
		};


class MyTriEdgeCollapse: public vcg::tri::TriEdgeCollapseQuadric< CMeshO, MyTriEdgeCollapse, QHelper > {
						public:
						typedef  vcg::tri::TriEdgeCollapseQuadric< CMeshO,  MyTriEdgeCollapse, QHelper> TECQ;
            typedef  CMeshO::VertexType::EdgeType EdgeType;
            inline MyTriEdgeCollapse(  const EdgeType &p, int i) :TECQ(p,i){}
};

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

CMeshO cm;
int main(int argc ,char**argv){
if(argc<4) Usage();
/*
 bool PreserveBoundary=true;
 float QualityThr=0.3;
 bool PreserveNormal=false;
 bool OptimalPlacement=true;*/
 bool CleaningFlag=false;
 int TargetFaceNum=atoi(argv[3]);
 MyTriEdgeCollapse::SetDefaultParams();
 TriEdgeCollapseQuadricParameter qparams=MyTriEdgeCollapse::Params();

 // parse command line.
 for(int i=4; i < argc;){
   if(argv[i][0]=='-')
     switch(argv[i][1]){ 
     case 'H' : MyTriEdgeCollapse::Params().SafeHeapUpdate=true; printf("Using Safe heap option\n"); break;
     case 'Q' : if(argv[i][2]=='y') { qparams.QualityCheck	= true;  printf("Using Quality Checking\n");	}
       else { qparams.QualityCheck	= false; printf("NOT Using Quality Checking\n");	}                break;		
     case 'N' : if(argv[i][2]=='y') { qparams.NormalCheck	= true;  printf("Using Normal Deviation Checking\n");	}
       else { qparams.NormalCheck	= false; printf("NOT Using Normal Deviation Checking\n");	}        break;		
     case 'O' : if(argv[i][2]=='y') { qparams.OptimalPlacement	= true;  printf("Using OptimalPlacement\n");	}
       else { qparams.OptimalPlacement	= false; printf("NOT Using OptimalPlacement\n");	}        break;		
     case 'S' : if(argv[i][2]=='y') { qparams.ScaleIndependent	= true;  printf("Using ScaleIndependent\n");	}
       else { qparams.ScaleIndependent	= false; printf("NOT Using ScaleIndependent\n");	}        break;		
     case 'B' : if(argv[i][2]=='y') { qparams.FastPreserveBoundary	= true;  printf("Preserving Boundary\n");	}
       else { qparams.FastPreserveBoundary	= false; printf("NOT Preserving Boundary\n");	}        break;		
     case 'T' : if(argv[i][2]=='y') { qparams.PreserveTopology	= true;  printf("Preserving Topology\n");	}
       else { qparams.PreserveTopology	= false; printf("NOT Preserving Topology\n");	}        break;		
     case 'q' :	qparams.QualityThr	= atof(argv[i]+2);	           printf("Setting Quality Thr to %f\n",atof(argv[i]+2)); 	 break;			
     case 'n' :	qparams.NormalThrRad = math::ToRad(atof(argv[i]+2));  printf("Setting Normal Thr to %f deg\n",atof(argv[i]+2)); break;	
     case 'b' :	qparams.BoundaryWeight  = atof(argv[i]+2);			printf("Setting Boundary Weight to %f\n",atof(argv[i]+2)); break;		
     case 'P' :	CleaningFlag=true;  printf("Cleaning mesh before simplification\n",atof(argv[i]+2)); break;	
       
     default  :  printf("Unknown option '%s'\n", argv[i]);
       exit(0);
     }
   i++;
 }


 MyTriEdgeCollapse::Params()=qparams;

 int err=vcg::tri::io::ImporterPLY<CMeshO>::Open(cm,argv[1]);
 printf("Mesh loaded Verts: %d Faces: %d \n",cm.vn,cm.fn);
 if(CleaningFlag){
      int dup = tri::Clean<CMeshO>::RemoveDuplicateVertex(cm);
      int unref =  tri::Clean<CMeshO>::RemoveUnreferencedVertex(cm);
      printf("Removed %i duplicate and %i unreferenced vertices from mesh \n",dup,unref);
  }


  printf("reducing it to %i\n",TargetFaceNum);



 tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(cm);
 tri::UpdateBounding<CMeshO>::Box(cm);
	
 vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(cm);

  cm.face.EnableVFAdjacency();
  tri::UpdateTopology<CMeshO>::VertexFace(cm);	
  tri::UpdateFlags<CMeshO>::FaceBorderFromNone(cm);

 math::Quadric<double> QZero;
  QZero.Zero();
  QuadricTemp TD(cm.vert);
  QHelper::TDp()=&TD;

  TD.Start(QZero);
 
  MyTriEdgeCollapse::SetHint(MyTriEdgeCollapse::HNHasVFTopology);
  MyTriEdgeCollapse::SetHint(MyTriEdgeCollapse::HNHasBorderFlag);

  
  
  
  vcg::LocalOptimization<CMeshO> DeciSession(cm);
   int t1=clock();	
   printf("Initializing simplification\n");
   DeciSession.Init<MyTriEdgeCollapse >();
   int t2=clock();	


   DeciSession.SetTargetSimplices(TargetFaceNum);
   DeciSession.SetTimeBudget(0.01f); // this allow to update the progress bar 10 time for sec...

  int startFn=cm.fn;
  int faceToDel=cm.fn-TargetFaceNum;
 while( DeciSession.DoOptimization() && cm.fn>TargetFaceNum )
 {
   printf("Percent %2d%%\r",(100-100*(cm.fn-TargetFaceNum)/(faceToDel)));
   fflush(stdout);
 };
 int t3=clock();
 DeciSession.Finalize<MyTriEdgeCollapse >();
 printf("mesh  %d %d Error %g \n",cm.vn,cm.fn,DeciSession.currMetric);
 printf("\nCompleted in (%i+%i) msec\n",t2-t1,t3-t2);
 


/*
  int FinalSize=atoi(argv[3]);
	//int t0=clock();	
   int err=vcg::tri::io::Importer<CMeshO>::Open(mesh,argv[1]);
  if(err) 
  {
    printf("Unable to open mesh %s : '%s'\n",argv[1],vcg::tri::io::Importer<CMeshO>::ErrorMsg(err));
    exit(-1);
  }

    
  TriEdgeCollapseQuadricParameter qparams;
  MyTriEdgeCollapse::SetDefaultParams();
  qparams.QualityThr  =.3;
  float TargetError=numeric_limits<float>::max();
  bool CleaningFlag =false;
  

 
	  math::Quadric<double> QZero;
  QZero.Zero();
  QuadricTemp TD(mesh.vert);
  QHelper::TDp()=&TD;

  TD.Start(QZero);
  MyTriEdgeCollapse::SetDefaultParams();
  MyTriEdgeCollapse::SetHint(MyTriEdgeCollapse::HNHasVFTopology);
  MyTriEdgeCollapse::SetHint(MyTriEdgeCollapse::HNHasBorderFlag);

  MyTriEdgeCollapse::Params().QualityThr=qparams.QualityThr;
  
*/
  /*
  if(PreserveBoundary) 
    MyTriEdgeCollapse::Params().FastPreserveBoundary=true;

  MyTriEdgeCollapse::Params().OptimalPlacement=OptimalPlacement;
  	
  if(PreserveNormal) {
	MyTriEdgeCollapse::Params().NormalCheck= true;
	MyTriEdgeCollapse::Params().NormalThrRad = M_PI/4.0;
	}
	else
	MyTriEdgeCollapse::Params().NormalCheck= false;
  
  vcg::LocalOptimization<CMeshO> DeciSession(mesh);
  int t1=clock();		
  DeciSession.Init<MyTriEdgeCollapse >();
  int t2=clock();	
  printf("Initial Heap Size %i\n",DeciSession.h.size());
  
  DeciSession.SetTargetSimplices(FinalSize);
  DeciSession.SetTimeBudget(0.1f); // this allow to update the progress bar 10 time for sec...
  
  // if(TargetError< numeric_limits<double>::max() ) DeciSession.SetTargetMetric(TargetError);
  int startFn=mesh.fn;
  int faceToDel=mesh.fn-FinalSize;
  printf("%d %d\n",mesh.fn,FinalSize);
  while(DeciSession.DoOptimization() && mesh.fn>FinalSize)
    printf("Current Mesh size %7i %2d%% done \r",mesh.fn,100-100*(mesh.fn-FinalSize)/(faceToDel));

 	

	DeciSession.Finalize<MyTriEdgeCollapse >();
  
  
  */
 
  vcg::tri::io::ExporterPLY<CMeshO>::Save(cm,argv[2]);
	return 0;

}

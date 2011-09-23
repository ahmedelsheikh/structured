#ifndef GLU_TESSELLATOR_CAP_H
#define GLU_TESSELLATOR_CAP_H
#include "glu_tesselator.h"

namespace vcg {
namespace tri {

// This function take a mesh with one or more boundary stored as edges, and fill another mesh with a triangulation of that boundaries.
// it assumes that boundary are planar and exploits glutessellator for the triangulaiton
template <class MeshType>
void CapEdgeMesh(MeshType &em, MeshType &cm)
{
  typedef typename MeshType::EdgeType EdgeType;
  std::vector< std::vector<Point3f> > outlines;
  std::vector<Point3f> outline;
  UpdateFlags<MeshType>::EdgeClearV(em);
  UpdateTopology<MeshType>::EdgeEdge(em);
  int nv=0;
  for(size_t i=0;i<em.edge.size();i++)
  {
    if (!em.edge[i].IsV())
    {
      EdgeType* startE=&(em.edge[i]);
      int startI = 0;
      int curI = startI;
      EdgeType* curE = startE;
      EdgeType* nextE; int nextI;
      do
      {
        curE->SetV();
        outline.push_back(curE->V(curI)->P());
        nextE=curE->EEp((curI+1)%2);
        nextI=curE->EEi((curI+1)%2);
        curE=nextE;
        curI=nextI;
        nv++;
      }
      while(curE != startE);

      outlines.push_back(outline);
      outline.clear();
    }
  }
  if (nv<2) return;
//  printf("Found %i outlines for a total of %i vertices",outlines.size(),nv);

  typename MeshType::VertexIterator vi=vcg::tri::Allocator<MeshType>::AddVertices(cm,nv);
  for (size_t i=0;i<outlines.size();i++)
  {
    for(size_t j=0;j<outlines[i].size();++j,++vi)
      (&*vi)->P()=outlines[i][j];
  }

  std::vector<int> indices;
  glu_tesselator::tesselate(outlines, indices);
  std::vector<Point3f> points;
  glu_tesselator::unroll(outlines, points);
  typename MeshType::FaceIterator fi=tri::Allocator<MeshType>::AddFaces(cm,nv-2);
  for (size_t i=0; i<indices.size(); i+=3,++fi)
  {
    (*&fi)->V(0)=&cm.vert[ indices[i+0] ];
    (*&fi)->V(1)=&cm.vert[ indices[i+1] ];
    (*&fi)->V(2)=&cm.vert[ indices[i+2] ];
  }
  Clean<MeshType>::RemoveDuplicateVertex(cm);
  UpdateBounding<MeshType>::Box(cm);
}

} // end namespace tri
} // end namespace vcg

#endif // GLU_TESSELLATOR_CAP_H
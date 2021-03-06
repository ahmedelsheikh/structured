/* * structured - Tools for the Generation and Visualization of Large-scale
 * Three-dimensional Reconstructions from Image Data. This software includes
 * source code from other projects, which is subject to different licensing,
 * see COPYING for details. If this project is used for research see COPYING
 * for making the appropriate citations.
 * Copyright (C) 2013 Matthew Johnson-Roberson <mattkjr@gmail.com>
 *
 * This file is part of structured.
 *
 * structured is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * structured is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with structured.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TightFit_H
#define TightFit_H
#include <osgUtil/Optimizer>
#include <string>
#include <osg/State>
#include <osg/Referenced>
#include <vips/vips>
#include <stdio.h>
class TightFitAtlasBuilder : public osgUtil::Optimizer::TextureAtlasBuilder , public osg::Referenced
{
public:

    TightFitAtlasBuilder(int mosaic_cell_num);


    // void addSource(vips::VImage &img);
    void buildAtlas(void);
    osg::ref_ptr<osg::Image> getImage(int index,int sizeIndex);

    unsigned int getNumAtlases(){return _atlasList.size();}
    osg::Image *getAtlasByNumber(unsigned int i){
        if(i < _atlasList.size())
            return _atlasList[i]->_image;
        return NULL;
    }
    int getMaxNumImagesPerAtlas(int tilewidth,int tileheight)

    {

        double area=((double)tilewidth + 2*_margin)*((double)tileheight + 2*_margin);
        if(area==0.0)
            return 0;
        return (int)floor((_maximumAtlasWidth*_maximumAtlasHeight)/area);
    }
    std::vector<void *> atlasSourceMatrix;
    std::vector<osg::Matrix> offsetMats;
};


class VipsAtlasBuilder : public TightFitAtlasBuilder
{
public:
    unsigned int getNumAtlases(){return _atlasList.size();}

    VipsAtlasBuilder(int POTAtlasSize,int mosaic_cell_num,int VTtileSize,int VToverlap,bool dryRun=false );
    int _VTtileSize,_VToverlap;
    osg::Matrix getTextureMatrix(vips::VImage *);
    int getAtlasHeight(){return _maximumAtlasHeight;}
    int getAtlasWidth(){return _maximumAtlasWidth;}
    bool _dryRun;
    void addSource(vips::VImage *img);
    void addSource(vips::VImage *img,vips::VImage *imglevel,int level);

    void buildAtlas();
    void completeRow(unsigned int indexAtlas);
class VAtlas;
     class VSource : public Source{
     public:
         VSource( vips::VImage * image): Source((const osg::Image*)NULL),
             _image(image) ,_atlas(NULL),_ds_image(NULL),_level(-1){width=_image->Xsize(); height=_image->Ysize();}
         VSource( vips::VImage * image,vips::VImage *ds_img,int level): Source((const osg::Image*)NULL),
             _image(image) ,_atlas(NULL),_ds_image(ds_img),_level(level){width=_image->Xsize(); height=_image->Ysize();}
      vips::VImage *_image;
      VAtlas *_atlas;
      int width,height;
      vips::VImage *_ds_image;
      int _level;
      bool suitableForAtlas(int maximumAtlasWidth, int maximumAtlasHeight);

     };
     typedef std::vector< osg::ref_ptr<VSource> > VSourceList;
     VSource* getSource(const vips::VImage *image);

     class VAtlas: public Atlas{
     public:
         VAtlas(int width, int height, int margin,int level=-1):Atlas(width,height,0),_image(NULL),_ds_image(NULL),_indexFirstOfRow(0),_level(level){if(margin>0){fprintf(stderr,"Cannot have margin in this implmentation being reset to 0 margin!\n");}}
         ~VAtlas();

         vips::VImage *_image;
         vips::VImage *_ds_image;

         VSourceList _sourceList;
         bool addSource(VSource* source);
       enum FitsIn
       {
	 DOES_NOT_FIT_IN_ANY_ROW,
	 FITS_IN_CURRENT_ROW,
	 IN_NEXT_ROW
       };	 
       FitsIn doesSourceFit(VSource* source);

         void copySources(bool dryRun=false);

         int _indexFirstOfRow;
         int _level;

     };
     VSourceList _sourceList;
     typedef std::vector< osg::ref_ptr<VAtlas> > VAtlasList;
     VAtlasList _atlasList;

     struct CompareSrc
     {
         bool operator()(osg::ref_ptr<VSource> src1, osg::ref_ptr<VSource> src2) const
         {
             return src1->_image->Xsize() > src2->_image->Ysize();
         }
     };
};

class FitChecker  : public VipsAtlasBuilder{
  public:
    FitChecker(std::vector<std::pair<int,int> > &imageSizes,int POTsize,double scale, int VTtileSize, int VToverlap, bool dryRun);
    std::vector<vips::VImage *> _tmp_images;
    ~FitChecker();
};
bool getScaleFactorForAtlasFit(float &scale,std::vector<std::pair<int,int> > &imageSizes,int &POTsize,int VTtileSize,int VToverlap);


#endif // TightFit_H

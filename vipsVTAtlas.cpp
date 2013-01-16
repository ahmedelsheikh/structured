#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/io_utils>
#include <osg/Timer>
#include <string>
#include <vips/vips.h>
#include "BuildAtlas.h"
#include "MemUtils.h"
using namespace std;
int main( int argc, char **argv )
{
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);
    osg::Timer_t before_computeMax = osg::Timer::instance()->tick();

    string matname;
    if(!arguments.read("-mat",matname)){
        fprintf(stderr, "need mat\n");
        exit(-1);
    }
    double scaleFactor=1.0;
    arguments.read("-scale",scaleFactor);
    double maxRes=pow(2,17);
    arguments.read("-maxRes",maxRes);
    int POTatlasSize;
    if(!arguments.read("-potsize",POTatlasSize)){
        fprintf(stderr,"-potsize required\n");
        exit(-1);
    }
    string basedir="mesh";
    arguments.read("-dir",basedir);

    string mosaic_cells_fname;
    if(!arguments.read("-cells",mosaic_cells_fname)){
        fprintf(stderr, "need mat\n");
        exit(-1);
    }
    int level = -1;
    int row=-1;
    arguments.read("-level",level);
    arguments.read("-row",row);
    bool flat=arguments.read("-flatatlas");
    int totalX,totalY;
    std::vector<mosaic_cell> mosaic_cells;
#if VIPS_MINOR_VERSION > 24

    vips_init(argv[0]);
#endif
    loadMosaicCells(mosaic_cells_fname,totalX,totalY,mosaic_cells);

    osg::Matrix viewProj;
    readMatrixToScreen(matname,viewProj);

        bool outputPyr=true;
        string imgFile;
        if(arguments.read("-img",imgFile))
            outputPyr=false;
            createVTAtlas( viewProj, totalX, totalY,POTatlasSize,
                         mosaic_cells,
                       outputPyr,scaleFactor,basedir,imgFile,flat,maxRes,level,row);
   osg::Timer_t after_computeMax = osg::Timer::instance()->tick();
   double totalTime=osg::Timer::instance()->delta_s(before_computeMax, after_computeMax);
   printf("Total Time vipsAtlas %s\n",format_elapsed(totalTime).c_str());

   //cout << "Time for vipsAtlas = " << osg::Timer::instance()->delta_s(before_computeMax, after_computeMax) <<endl;


}

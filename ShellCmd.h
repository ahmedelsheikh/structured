#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <stdlib.h>
#include <vector>
#include "stereo_cells.hpp"
using namespace std;
class ShellCmd{
public:
  ShellCmd(const char *basepath,float simp_mult,const char *pos_simp_log_dir,bool dist_run,const char *cwd,const char *aggdir,bool have_mb_ply):basepath(basepath),simp_mult(simp_mult),pos_simp_log_dir(pos_simp_log_dir),dist_run(dist_run),cwd(cwd),aggdir(aggdir),have_mb_ply(have_mb_ply){};
  void pos_dice(vector<Cell_Data>cells,float eps);

  void pos_simp_cmd(bool run);
 void pos_simp_cmd2(bool run);
  const char *basepath;
  float simp_mult;
  const char *pos_simp_log_dir;

  bool dist_run;
const char *cwd,*aggdir;
  bool have_mb_ply;
};

void save_bbox_frame (GtsBBox * bb, FILE * fptr);
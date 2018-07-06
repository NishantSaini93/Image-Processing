
#include <opencv2/opencv.hpp>
#include<fstream>
#include <utility>
#include<queue>
#include<vector>


  using namespace cv;
using namespace std;
// checking is the pixel already present in path or not
int check(int x, vector < int > cpath) {
  int size = cpath.size();
  for (int i = 0; i < size; i++)
    if (cpath[i] == x)
      return 0;
  return 1;
}

int check2(vector<int> tar,int la)
{
int x1=la;
for(int i=0;i<tar.size();i++)
{
if(tar[i]==x1)
return 1;
}
return 0;

}
int main(int argc, char * * argv) {
  if (argc != 4) {
    cout << "Usage: ../seg input_image initialization_file output_mask" << endl;
    return -1;
  }

  // Load the input image
  // the image should be a 3 channel image by default but we will double check that in teh seam_carving
  Mat in_image;
  in_image = imread(argv[1] /*, CV_LOAD_IMAGE_COLOR*/ );

  if (!in_image.data) {
    cout << "Could not load input image!!!" << endl;
    return -1;
  }

  if (in_image.channels() != 3) {
    cout << "Image does not have 3 channels!!! " << in_image.depth() << endl;
    return -1;
  }

  // the output image
  Mat out_image = in_image.clone();

  ifstream f(argv[2]);
  if (!f) {
    cout << "Could not load initial mask file!!!" << endl;
    return -1;
  }

  //calculate the pixel energies
  int width = in_image.cols;
  int height = in_image.rows;

  Mat src, src_gray;
  Mat gradient;
  Mat weightss;
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  src = in_image.clone();
 

  cvtColor(src, src_gray, CV_BGR2GRAY);
  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_x, abs_grad_x);
  convertScaleAbs(grad_y, abs_grad_y);
  addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradient);
// getting max energy of pixel in image
  int max = 0;
  int count = 0;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      count++;
      int temp1 = src_gray.at < uchar > (i, j);

      if (max < temp1) {
        max = temp1;
      }

    }
  }

  vector < vector < pair < int, int > > > adlis;
  adlis.resize(count);
//calculate weights by taking pixel and pixel toward its left,right,down,up
  for (int id = 0; id < count; id++) {
    int i = id / width;
    int j = id % width;

    if (id == 0) {
      int weightssd = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i + 1, j)) / 2);
      int weightssr = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j + 1)) / 2);

      pair < int, int > npair1 = make_pair((i) * width + j + 1, weightssr);
      adlis[id].push_back(npair1);
      pair < int, int > npair = make_pair((i + 1) * width + j, weightssd);
      adlis[id].push_back(npair);

    } else if (id == width - 1) {

      int weightssd = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i + 1, j)) / 2);

      int weightssl = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j - 1)) / 2);
      pair < int, int > npair = make_pair((i + 1) * width + j, weightssd);
      adlis[id].push_back(npair);
      pair < int, int > npair1 = make_pair((i) * width + j - 1, weightssl);
      adlis[id].push_back(npair1);

    } else if (id == width * (height - 1)) {

      int weightssu = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i - 1, j)) / 2);
      int weightssr = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j + 1)) / 2);

      pair < int, int > npair = make_pair((i - 1) * width + j, weightssu);
      adlis[id].push_back(npair);
      pair < int, int > npair1 = make_pair((i) * width + j + 1, weightssr);
      adlis[id].push_back(npair1);
    } else if (id == count - 1) {
      int weightssu = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i - 1, j)) / 2);
      int weightssl = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j - 1)) / 2);
      pair < int, int > npair1 = make_pair((i) * width + j - 1, weightssl);
      adlis[id].push_back(npair1);
      pair < int, int > npair = make_pair((i - 1) * width + j, weightssu);
      adlis[id].push_back(npair);

    } else if (id / width == 0) {
      int weightssd = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i + 1, j)) / 2);

      int weightssl = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j - 1)) / 2);
      int weightssr = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j + 1)) / 2);

      pair < int, int > npair1 = make_pair((i) * width + j + 1, weightssr);
      adlis[id].push_back(npair1);

      pair < int, int > npair2 = make_pair((i + 1) * width + j, weightssd);
      adlis[id].push_back(npair2);

      pair < int, int > npair3 = make_pair((i) * width + j - 1, weightssl);
      adlis[id].push_back(npair3);

    } else if ((id + 1) % width == 0) {
      int weightssd = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i + 1, j)) / 2);
      int weightssu = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i - 1, j)) / 2);
      int weightssl = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j - 1)) / 2);

      pair < int, int > npair = make_pair((i - 1) * width + j, weightssu);
      adlis[id].push_back(npair);

      pair < int, int > npair2 = make_pair((i + 1) * width + j, weightssd);
      adlis[id].push_back(npair2);

      pair < int, int > npair3 = make_pair((i) * width + j - 1, weightssl);
      adlis[id].push_back(npair3);
    } else if (id % width == 0) {
      int weightssd = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i + 1, j)) / 2);
      int weightssu = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i - 1, j)) / 2);

      int weightssr = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j + 1)) / 2);
      pair < int, int > npair = make_pair((i - 1) * width + j, weightssu);
      adlis[id].push_back(npair);

      pair < int, int > npair1 = make_pair((i) * width + j + 1, weightssr);
      adlis[id].push_back(npair1);

      pair < int, int > npair2 = make_pair((i + 1) * width + j, weightssd);
      adlis[id].push_back(npair2);

    } else if (id > (width * (height - 1))) {

      int weightssu = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i - 1, j)) / 2);
      int weightssl = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j - 1)) / 2);
      int weightssr = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j + 1)) / 2);
      pair < int, int > npair = make_pair((i - 1) * width + j, weightssu);
      adlis[id].push_back(npair);

      pair < int, int > npair1 = make_pair((i) * width + j + 1, weightssr);
      adlis[id].push_back(npair1);

      pair < int, int > npair3 = make_pair((i) * width + j - 1, weightssl);
      adlis[id].push_back(npair3);

    } else {

      int weightssd = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i + 1, j)) / 2);
      int weightssu = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i - 1, j)) / 2);
      int weightssl = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j - 1)) / 2);
      int weightssr = max - ((gradient.at < uchar > (i, j) + gradient.at < uchar > (i, j + 1)) / 2);
      pair < int, int > npair = make_pair((i - 1) * width + j, weightssu);
      adlis[id].push_back(npair);

      pair < int, int > npair1 = make_pair((i) * width + j + 1, weightssr);
      adlis[id].push_back(npair1);

      pair < int, int > npair2 = make_pair((i + 1) * width + j, weightssd);
      adlis[id].push_back(npair2);

      pair < int, int > npair3 = make_pair((i) * width + j - 1, weightssl);
      adlis[id].push_back(npair3);

    }

  }

 //calculating total number of pixels
int ncont=0;
int totl=0;
for(int i=0;i<count;i++)
{for(int j=0;j<adlis[i].size();j++)
{
totl=totl+adlis[i][j].second;
ncont++;
}

}

int avr=totl/ncont;


for(int i=0;i<count;i++)
{for(int j=0;j<adlis[i].size();j++)
{
 if(adlis[i][j].second<avr)
   {adlis[i][j].second=1;}
else{
adlis[i][j].second=1000000;
}

}


}

  

  int n;
  f >> n;



 vector<int> source;
vector<int> target;
  for (int i = 0; i < n; ++i)

  {
    int x, y, t;
    f >> x >> y >> t;
      
       

    if (x < 0 || x >= width || y < 0 || y >= height) {
      cout << "I valid pixel mask!" << endl;
      return -1;
    }

    

    

    if (t == 1) {
      source.push_back((width * y) + x);
      
    } else {
      target.push_back((width * y) + x);
    }

    
  }

 

  

  vector < pair < int, int > > comb2;
  for (int i = 0; i < count; i++) {
    pair < int, int > npair = make_pair(i, 0);
    comb2.push_back(npair);
  }
  

  vector < pair < int, int > > comb;
  for (int i = 0; i < count; i++) {
    pair < int, int > npair = make_pair(i, 0);
    comb.push_back(npair);
  }

  int label;
  queue < vector < int > > q;
  vector < int > path;
 
for(int i=0;i<source.size();i++)
{vector<int> path0;
path0.push_back(source[i]);
q.push(path0);

}
 vector<int>::iterator it;



  // min cut max flow 

   bool v;
  v=true;
while(v==true)
  {v=false;
  while (!q.empty()) {
    again: vector < int > cpath;
    cpath = q.front();
    q.pop();
    int last = cpath.back();

   
      if (check2(target,last)==1) {
        
        path.clear();

        path = cpath;
        v=true;

        comb.clear();
        for (int i = 0; i < count; i++) {
          pair < int, int > npair = make_pair(i, 0);
          comb.push_back(npair);
        }
         while(!q.empty())
        {q.pop();
        }
           for(int i=0;i<source.size();i++)
            {vector<int> path0;
            path0.push_back(source[i]);
                 q.push(path0);

           }
 
        
        break;
        }
        
     
       

    for (int i = 0; i < adlis[last].size(); ++i) {
      if ( check(adlis[last][i].first,cpath) && adlis[last][i].second > 0  && comb[adlis[last][i].first].second == 0  ) {
            
        vector < int > opath(cpath);
        opath.push_back(adlis[last][i].first);
         
        comb[adlis[last][i].first].second = 1;
        
        q.push(opath);
        

      }

  }
}
  
  
   if(v==true)
      {
           int smallest=99999999;

        for (int i = 0; i < path.size() - 1; i++) {
          for (int j = 0; j < adlis[path[i]].size(); j++) {
            if (adlis[path[i]][j].first == path[i + 1]) {
              if (adlis[path[i]][j].second < smallest) {
                    
                        smallest = adlis[path[i]][j].second;
                          
                     
                  

              }
            }
          }

        }
         

     
         
        
        
        for (int i = 0; i < path.size() - 1; i++) {
          for (int j = 0; j < adlis[path[i]].size(); j++) {
            if (adlis[path[i]][j].first == path[i + 1]) {
              adlis[path[i]][j].second = adlis[path[i]][j].second - smallest;

            }
          }

        }
         
         
       
        for (int i = 0; i < path.size() - 1; i++) {
          for (int j = 0; j < adlis[path[i+1]].size(); j++) {
            if (adlis[path[i + 1]][j].first == path[i]) {
              adlis[path[i + 1]][j].second = adlis[path[i + 1]][j].second + smallest;

            }
          }

        }

      
      
           
      }
      

    }
  
   

 for(int i=0;i<height;i++)
  {	for(int j=0;j<width;j++)
  	{
  	  Vec3b pixel;
          pixel[0] = 255;
          pixel[1] = 0;
          pixel[2] = 0;
          
         
          
          out_image.at<Vec3b>(i, j) = pixel;
  	}
  	
  }

  vector < pair < int, int > > comb1;
  for (int i = 0; i < count; i++) {
    pair < int, int > npair = make_pair(i, 0);
    comb1.push_back(npair);
  }

  Vec3b pixel2;
  pixel2[0] =0;
  pixel2[1] = 0;
  pixel2[2] = 255;

  queue < int > qcolor;
  int u = source[0];
  qcolor.push(u);
  int x1, x2;
  x1 = u / width;
  x2 = u % width;
  out_image.at < Vec3b > (x1, x2) = pixel2;
  int mino=99999999;
  
//performing BFS
  while (!qcolor.empty()) {

    int last = qcolor.front();
    qcolor.pop();

    for (int i = 0; i < adlis[last].size(); ++i) {
      if (comb1[adlis[last][i].first].second == 0 && adlis[last][i].second>0) {
            
            Vec3b pixel2;
              pixel2[0] = 0;
             pixel2[1] = 0;
        pixel2[2] = 225;
        int x3, x4;
        x3 = (adlis[last][i].first) / width;
        x4 = (adlis[last][i].first) % width;
        out_image.at < Vec3b > (x3, x4) = pixel2;

        qcolor.push(adlis[last][i].first);
        comb1[adlis[last][i].first].second = 1;

      }

    }
  }


    // write it on disk
    imwrite(argv[3], out_image);

  // also display them both

  namedWindow("Original image", WINDOW_AUTOSIZE);
  namedWindow("Show Marked Pixels", WINDOW_AUTOSIZE);
  imshow("Original image", in_image);
  imshow("Show Marked Pixels", out_image);
  waitKey(0);
  return 0;
}

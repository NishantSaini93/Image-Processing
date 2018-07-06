
#include "sc.h"

using namespace cv;
using namespace std;
#include<cmath>
#include<stack>




bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image){

    // some sanity checks
    // Check 1 -> new_width <= in_image.cols
    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }
    
    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;

    }
    
    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;
        
    }

    
    return seam_carving_trivial(in_image, new_width, new_height, out_image);
}


// seam carves by removing trivial seams
bool seam_carving_trivial(Mat& in_image, int new_width, int new_height, Mat& out_image){

    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();
 while(iimage.rows!=new_height || iimage.cols!=new_width){
        // horizontal seam if needed
        if(iimage.rows>new_height){
            reduce_horizontal_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
        
        if(iimage.cols>new_width){
            reduce_vertical_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
    }
    
    
    out_image = oimage.clone();
    return true;
}

// horizontl trivial seam is a seam through the center of the image
bool reduce_horizontal_seam_trivial(Mat& in_image, Mat& out_image){

    // retrieve the dimensions of the new image
    int rows = in_image.rows;
    int cols = in_image.cols;
    
    // create an image slighly smaller
    out_image = Mat(rows-1, cols, CV_8UC3);
//calculating energy of image 
   Mat src, src_gray;
  	Mat energy,energyi;
	int scale = 1;
  	int delta = 0;
         int ddepth = CV_16S;
	src = in_image.clone();
   	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

        cvtColor( src, src_gray, CV_BGR2GRAY );
	Mat grad_x, grad_y;
  	Mat abs_grad_x, abs_grad_y;
	
         Sobel( src_gray, grad_x, ddepth, 1, 0,3, scale, delta, BORDER_DEFAULT );
         Sobel( src_gray, grad_y, ddepth, 0, 1,3,scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );
	convertScaleAbs( grad_y, abs_grad_y );
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, energy);



int tt=0;
int tt1=0;

        
        int val[rows][cols];
    
          
        for(int i=0;i<=rows-1;++i)

        {
              val[i][cols-1]=abs((double)energy.at<char>(i,cols-1));

        }

        for(int i=cols-2;i>=0;--i)
        {

            for(int j=0;j<=rows-1;++j)
            {
              if(j==0)
                  {val[j][i]=min(abs((double)energy.at<char>(j,i)-(double)energy.at<char>(j,i+1))+val[j][i+1],abs((double)energy.at<char>(j,i)-(double)energy.at<char>(j+1,i+1))+val[j+1][i+1]);
                  }

                 else if(j==rows-1)
                     {val[j][i]=min(abs((double)energy.at<char>(j,i)-(double)energy.at<char>(j,i+1))+val[j][i+1],abs((double)energy.at<char>(j,i)-(double)energy.at<char>(j-1,i+1))+val[j-1][i+1]);
                     } 
                   else
                       { val[j][i]=min(abs((double)energy.at<char>(j,i)-(double)energy.at<char>(j,i+1))+val[j][i+1],min(abs((double)energy.at<char>(j,i)-(double)energy.at<char>(j+1,i+1))+val[j+1][i+1],abs((double)energy.at<char>(j,i)-(double)energy.at<char>(j-1,i+1))+val[j-1][i+1]));
                         
                        }
                          

            }

         }

int temp2;

int temp=val[0][0];


//Identifying the lowest evergy value in leftmost column

for(int j=0;j<rows;++j)
      {

            if(temp>val[j][0])
            {
            temp=val[j][0];
            tt1=j;
            }
     }


stack<int> s;//statck to save the seam
s.push(tt1);
//saving coordinates of seam pixel
for(int i=0;i<cols-1;++i)
      {
        if(tt1==0)
                  {temp2=min(val[tt1][i+1],val[tt1+1][i+1]);
  
                    if(temp2==val[tt1+1][i+1])
                        {tt1=tt1+1;
                        }
                       s.push(tt1);

                   }
      
           else if(tt1==rows-1)
                    {temp2=min(val[tt1][i+1],val[tt1-1][i+1]);
                       if(temp2==val[tt1-1][i+1])
                       {tt1=tt1-1;
                       }
                     s.push(tt1);
                     }
               else
               {

                     temp2=min(val[tt1][i+1],min(val[tt1-1][i+1],val[tt1+1][i+1]));
                      if(temp2==val[tt1-1][i+1])
                     {tt1=tt1-1;
                       
                       }
                      else if(temp2==val[tt1+1][i+1])
                        {tt1=tt1+1;
                         }
                s.push(tt1);

               }
} 

//removing seam horizontally
for (int j = cols-1; j>=0; --j)
{
	if (s.top()<rows-1)
                        {
		for (int i = 0;i<s.top();++i)
                                           {
			
             Vec3b pixel=in_image.at<Vec3b>(i,j);
              out_image.at<Vec3b>(i,j)=pixel;		
		                             }
		for (int i = s.top()+1;i<rows;++i) 
                                                    {
			
		                                   
Vec3b pixel=in_image.at<Vec3b>(i,j);
              out_image.at<Vec3b>(i-1,j)=pixel; }
	                  }
                    
	else {
		for (int i = 0;i<rows-1;++i)
                                           {
					
		                          Vec3b pixel=in_image.at<Vec3b>(i,j);
              out_image.at<Vec3b>(i,j)=pixel;	  }
	      }
s.pop();
 }




    return true;
}


bool reduce_vertical_seam_trivial(Mat& in_image, Mat& out_image){
    
    int rows = in_image.rows;
    int cols = in_image.cols;
    
   
    out_image = Mat(rows, cols-1, CV_8UC3);
    
   Mat src, src_gray;
  	Mat energy,energyi;
	int scale = 1;
  	int delta = 0;
         int ddepth = CV_16S;
	src = in_image.clone();
   	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
       
         cvtColor( src, src_gray, CV_BGR2GRAY );
	Mat grad_x, grad_y;
  	Mat abs_grad_x, abs_grad_y;
        
          Sobel( src_gray, grad_x, ddepth, 1, 0,3, scale, delta, BORDER_DEFAULT );
         Sobel( src_gray, grad_y, ddepth, 0, 1,3,scale, delta, BORDER_DEFAULT );

	convertScaleAbs( grad_x, abs_grad_x );
	convertScaleAbs( grad_y, abs_grad_y );
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, energy );
      
     
    
	int val[rows][cols];

    

	for(int i=0;i<cols;++i)     {
             val[0][i]=abs((int)energy.at<char>(0,i));
           
       }
    
    for(int i=1;i<rows;++i){
            for(int j=0;j<cols;++j){
            if(j==0)
               {val[i][j]=min(abs(energy.at<char>(i,j)-energy.at<char>(i-1,j))+val[i-1][j],abs(energy.at<char>(i,j)-energy.at<char>(i-1,j+1))+val[i-1][j+1]);
               }

              else if(j==cols-1)
               {val[i][j]=min(abs(energy.at<char>(i,j)-energy.at<char>(i-1,j))+val[i-1][j],abs(energy.at<char>(i,j)-energy.at<char>(i-1,j-1))+val[i-1][j-1]);
               }

             else

                  {val[i][j]=min(abs(energy.at<char>(i,j)-energy.at<char>(i-1,j))+val[i-1][j],min(abs(energy.at<char>(i,j)-energy.at<char>(i-1,j-1))+val[i-1][j-1],abs(energy.at<char>(i,j)-energy.at<char>(i-1,j+1))+val[i-1][j+1]));

                  }

                  
                                   }
                          }


int temp;int temp2;
temp=val[rows-1][0];
int tt=0;


//finding meanimum in topmost row of image

	for(int i=0;i<cols;++i)
      {

            if(temp>val[rows-1][i])
            {
            temp=val[rows-1][i];
             tt=i;
             }
      }

stack<int> s;
s.push(tt);


for(int i=rows-1;i>0;--i)
      {
               if(tt==0)
                  {temp2=min(val[i-1][tt],val[i-1][tt+1]);
  
                    if(temp2==val[i-1][tt+1])
                        {tt=tt+1;
                        }
                       s.push(tt);

                     }
      
                 else  if(tt==cols-1)
                    {temp2=min(val[i-1][tt],val[i-1][tt-1]);
                       if(temp2==val[i-1][tt-1])
                       {tt=tt-1;
                       }
                     s.push(tt);
                     }
               else
               {

                     temp2=min(val[i-1][tt],min(val[i-1][tt-1],val[i-1][tt+1]));
                      if(temp2==val[i-1][tt-1])
                         {tt=tt-1;
                    
                         }
                       else if(temp2==val[i-1][tt+1])
                       {tt=tt+1;
                      
                       }
                       s.push(tt);
                

               }

}




for (int i = 0; i< rows; ++i)
{
	if (s.top()<cols-1)
                        {
		for (int j = 0;j<s.top();++j)
                                           {
                       Vec3b pixel = in_image.at<Vec3b>(i, j);
            

			out_image.at<Vec3b>(i,j) = pixel;		
		                             }
                
		for (int j = s.top()+1;j<cols;++j) 
                                                    {
			Vec3b pixel = in_image.at<Vec3b>(i,j);	
                         out_image.at<Vec3b>(i,j-1)=pixel;	
		                                    }
	                 }
	else {
		for (int j = 0;j<cols-1;++j)
                                           {
			Vec3b pixel = in_image.at<Vec3b>(i,j);	
                        out_image.at<Vec3b>(i,j)=pixel;	
		                            }
	     }

s.pop();
 }


    return true;
}

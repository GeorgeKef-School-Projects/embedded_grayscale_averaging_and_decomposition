#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define N 330 /* frame dimension for QCIF format */
#define M 440 /* frame dimension for QCIF format */
#define filename "dog_440x330.yuv"
#define file_yuv "akiyo2yuv.yuv"


/* code for armulator*/
#pragma arm section zidata="ram"
int current_y[N][M];
int current_u[N/2][M/2];
int current_v[N/2][M/2];
int resize_u[N][M];
int resize_v[N][M];
//int current_r[N][M];
//int current_g[N][M];
//int current_b[N][M];
#pragma arm section

#pragma arm section zidata="sram"
int buffer_yuv[3];
int buffer_rgb[3];
int gray;
#pragma arm section

int i,j;

void read(){
FILE *frame_c;
  if((frame_c=fopen(filename,"rb"))==NULL)
  {
    printf("current frame doesn't exist\n");
    exit(-1);
  }

  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      current_y[i][j]=fgetc(frame_c);
    }
  }

  for(i=0;i<N/2;i++)
  {
    for(j=0;j<M/2;j++)
    {
      current_u[i][j]=fgetc(frame_c);

      //Resizing of the current_u matrix
      resize_u[(i*2)+0][(j*2)+0] = current_u[i][j];
      resize_u[(i*2)+0][(j*2)+1] = current_u[i][j];
      resize_u[(i*2)+1][(j*2)+0] = current_u[i][j];
      resize_u[(i*2)+1][(j*2)+1] = current_u[i][j];
    }
  }

  for(i=0;i<N/2;i++)
  {
    for(j=0;j<M/2;j++)
    {
      current_v[i][j]=fgetc(frame_c);

      //Resizing of the current_v matrix
      resize_v[(i*2)+0][(j*2)+0] = current_v[i][j];
      resize_v[(i*2)+0][(j*2)+1] = current_v[i][j];
      resize_v[(i*2)+1][(j*2)+0] = current_v[i][j];
      resize_v[(i*2)+1][(j*2)+1] = current_v[i][j];
    }
  }

  fclose(frame_c);
}


void write()
{
  FILE *frame_yuv;
  frame_yuv=fopen(file_yuv,"wb");

  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      fputc(current_y[i][j],frame_yuv);
    }
  }

  for(i=0;i<N/2;i++)
  {
    for(j=0;j<M/2;j++)
    {
      fputc(resize_u[i*2][j*2],frame_yuv);
      //fputc(current_u[i][j],frame_yuv);
    }
  }

  for(i=0;i<N/2;i++)
  {
    for(j=0;j<M/2;j++)
    {
      fputc(resize_v[i*2][j*2],frame_yuv);
      //fputc(current_v[i][j],frame_yuv);
    }
  }
  fclose(frame_yuv);
}

void grayscale_averaging()
{
//  int B = 10; 
//  int x,y;
//  if(N<=330 && M<=440)
//  {
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      buffer_yuv[0] = current_y[i][j];
      buffer_yuv[1] = resize_u[i][j];
      buffer_yuv[2] = resize_v[i][j];
      
      /*
      //Loop tiling
      for(i=x;i<(x+B);i++)
      {
        for(j=y;j<(y+B);j++)
        {
      */
      //YUV to RGB
      buffer_rgb[0] = buffer_yuv[0]  + (1.370705 * (buffer_yuv[2] - 128));
      buffer_rgb[1] = buffer_yuv[0]  - (0.698001 * (buffer_yuv[2] - 128)) - (0.337633 * (buffer_yuv[1] -128));
      buffer_rgb[2] = buffer_yuv[0]  + (1.732446 * (buffer_yuv[1] - 128));

      if(buffer_rgb[0] < 0 )
      {
        buffer_rgb[0] = 0;
      }else if(buffer_rgb[0] > 255)
      {
        buffer_rgb[0] = 255;
      }

      if(buffer_rgb[1] < 0 )
      {
        buffer_rgb[1] = 0;
      }else if(buffer_rgb[1] > 255)
      {
        buffer_rgb[1] = 255;
      }

      if(buffer_rgb[2] < 0 )
      {
        buffer_rgb[2] = 0;
      }else if(buffer_rgb[2] > 255)
      {
        buffer_rgb[2] = 255;
      }
      
      //Averaging
      gray = (buffer_rgb[0] +  buffer_rgb[1] +  buffer_rgb[2])/3;
      //End of Avereging

      //Grayscale 
      //buffer_rgb[0] = gray;
      //buffer_rgb[1] = gray;
      //buffer_rgb[2] = gray;

      //RGB to YUV
      current_y[i][j] = 0.2988*gray + 0.5868*gray + 0.1143*gray;
      resize_u[i][j]  = 128 - 0.1724*gray - 0.3387*gray + 0.5112*gray;
      resize_v[i][j]  = 128 + 0.5118*gray - 0.4283*gray - 0.0834*gray;    

      //Loop Unrolling 
/*
      buffer_yuv[0] = current_y[i+1][j+1];
      buffer_yuv[1] = resize_u[i+1][j+1];
      buffer_yuv[2] = resize_v[i+1][j+1];

      //YUV to RGB
      buffer_rgb[0] = buffer_yuv[0]  + (1.370705 * (buffer_yuv[2] - 128));
      buffer_rgb[1] = buffer_yuv[0]  - (0.698001 * (buffer_yuv[2] - 128)) - (0.337633 * (buffer_yuv[1] -128));
      buffer_rgb[2] = buffer_yuv[0]  + (1.732446 * (buffer_yuv[1] - 128));

      if(buffer_rgb[0] < 0 )
      {
        buffer_rgb[0] = 0;
      }else if(buffer_rgb[0] > 255)
      {
        buffer_rgb[0] = 255;
      }

      if(buffer_rgb[1] < 0 )
      {
        buffer_rgb[1] = 0;
      }else if(buffer_rgb[1] > 255)
      {
        buffer_rgb[1] = 255;
      }

      if(buffer_rgb[2] < 0 )
      {
        buffer_rgb[2] = 0;
      }else if(buffer_rgb[2] > 255)
      {
        buffer_rgb[2] = 255;
      }
      
      //Averaging
      gray = (buffer_rgb[0] +  buffer_rgb[1] +  buffer_rgb[2])/3;
      //End of Avereging

      //Grayscale 
      //buffer_rgb[0] = gray;
      //buffer_rgb[1] = gray;
      //buffer_rgb[2] = gray;

      //RGB to YUV
      current_y[i+1][j+1] = 0.2988*gray + 0.5868*gray + 0.1143*gray;
      resize_u[i+1][j+1]  = 128 - 0.1724*gray - 0.3387*gray + 0.5112*gray;
      resize_v[i+1][j+1]  = 128 + 0.5118*gray - 0.4283*gray - 0.0834*gray; 
*/
        //}
      //}
    }
  }
  //}
}

void grayscale_decomposition()
{
  //int B = 10;
  //int x,y;
  //if(N<=330 && M<=440)
  //{
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      buffer_yuv[0] = current_y[i][j];
      buffer_yuv[1] = resize_u[i][j];
      buffer_yuv[2] = resize_v[i][j];
      /*
      //Loop tiling
      for(i=x;i<(x+B);i++)
      {
        for(j=y;j<(y+B);j++)
        {
      */

      //YUV to RGB
      buffer_rgb[0] = buffer_yuv[0]  + (1.370705 * (buffer_yuv[2] - 128));
      buffer_rgb[1] = buffer_yuv[0]  - (0.698001 * (buffer_yuv[2] - 128)) - (0.337633 * (buffer_yuv[1] -128));
      buffer_rgb[2] = buffer_yuv[0]  + (1.732446 * (buffer_yuv[1] - 128));

      if(buffer_rgb[0] < 0 )
      {
        buffer_rgb[0] = 0;
      }else if(buffer_rgb[0] > 255)
      {
        buffer_rgb[0] = 255;
      }

      if(buffer_rgb[1] < 0 )
      {
        buffer_rgb[1] = 0;
      }else if(buffer_rgb[1] > 255)
      {
        buffer_rgb[1] = 255;
      }

      if(buffer_rgb[2] < 0 )
      {
        buffer_rgb[2] = 0;
      }else if(buffer_rgb[2] > 255)
      {
        buffer_rgb[2] = 255;
      }

      //Decomposition
      gray = buffer_rgb[0];
      if(gray > buffer_rgb[1])
      //if(gray < buffer_rgb[1])
      {
        gray = buffer_rgb[1];
      }
      else if(gray > buffer_rgb[2])
      //else if(gray < buffer_rgb[2])
      {
        gray = buffer_rgb[2];
      }
      //End of Decomposition

      //Grayscale 
      //buffer_rgb[0] = gray;
      //buffer_rgb[1] = gray;
      //buffer_rgb[2] = gray;

      //RGB to YUV
      current_y[i][j] = 0.2988*gray + 0.5868*gray + 0.1143*gray;
      resize_u[i][j]  = 128 - 0.1724*gray - 0.3387*gray + 0.5112*gray;
      resize_v[i][j]  = 128 + 0.5118*gray - 0.4283*gray - 0.0834*gray; 

      //Loop Unrolling 
/*
      buffer_yuv[0] = current_y[i+1][j+1];
      buffer_yuv[1] = resize_u[i+1][j+1];
      buffer_yuv[2] = resize_v[i+1][j+1];

      //YUV to RGB
      buffer_rgb[0] = buffer_yuv[0]  + (1.370705 * (buffer_yuv[2] - 128));
      buffer_rgb[1] = buffer_yuv[0]  - (0.698001 * (buffer_yuv[2] - 128)) - (0.337633 * (buffer_yuv[1] -128));
      buffer_rgb[2] = buffer_yuv[0]  + (1.732446 * (buffer_yuv[1] - 128));

      if(buffer_rgb[0] < 0 )
      {
        buffer_rgb[0] = 0;
      }else if(buffer_rgb[0] > 255)
      {
        buffer_rgb[0] = 255;
      }

      if(buffer_rgb[1] < 0 )
      {
        buffer_rgb[1] = 0;
      }else if(buffer_rgb[1] > 255)
      {
        buffer_rgb[1] = 255;
      }

      if(buffer_rgb[2] < 0 )
      {
        buffer_rgb[2] = 0;
      }else if(buffer_rgb[2] > 255)
      {
        buffer_rgb[2] = 255;
      }

      //Decomposition*****
      gray = buffer_rgb[0];
      //Min: if(gray > buffer_rgb[1])
      if(gray < buffer_rgb[1])
      {
        gray = buffer_rgb[1];
      }
      //Min: else if(gray > buffer_rgb[2])
      else if(gray < buffer_rgb[2])
      {
        gray = buffer_rgb[2];
      }
      //End of Decomposition*****

      //Grayscale 
      //buffer_rgb[0] = gray;
      //buffer_rgb[1] = gray;
      //buffer_rgb[2] = gray;

      //RGB to YUV
      current_y[i+1][j+1] = 0.2988*gray + 0.5868*gray + 0.1143*gray;
      resize_u[i+1][j+1]  = 128 - 0.1724*gray - 0.3387*gray + 0.5112*gray;
      resize_v[i+1][j+1]  = 128 + 0.5118*gray - 0.4283*gray - 0.0834*gray; 
*/
        //}
      //}
    }
  }
  //}
}

int main(){
  
  read();

  grayscale_averaging();
  //grayscale_decomposition();

  write();

}
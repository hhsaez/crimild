/**
 * Copyright (c) 2013, Hugo Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "SDFGenerator.hpp"

using namespace crimild;
using namespace crimild::tools::fontgen;

SDFGenerator::SDFGenerator( void )
{

}

SDFGenerator::~SDFGenerator( void )
{

}

void SDFGenerator::execute( std::string source )
{
	ImageTGA input( source );

	std::cout << input.getWidth() << "x" << input.getHeight() << "x" << input.getBpp() << std::endl;

	const int INPUT_WIDTH = input.getWidth();
	const int INPUT_HEIGHT = input.getHeight();

	std::vector< short > xDist( INPUT_WIDTH * INPUT_HEIGHT );
	std::vector< short > yDist( INPUT_WIDTH * INPUT_HEIGHT );
	std::vector< double > gX( INPUT_WIDTH * INPUT_HEIGHT );
	std::vector< double > gY( INPUT_WIDTH * INPUT_HEIGHT );
	std::vector< double > data( INPUT_WIDTH * INPUT_HEIGHT );
	std::vector< double > outside( INPUT_WIDTH * INPUT_HEIGHT );
	std::vector< double > inside( INPUT_WIDTH * INPUT_HEIGHT );

    double inputMin = 255.0;
    double inputMax = -255.0;
    unsigned char *inputData = input.getData();

    for ( int i = 0; i < INPUT_WIDTH * INPUT_HEIGHT; i++ ) {
        double v = inputData[ i ];
        data[ i ] = v;
        if ( v > inputMax ) inputMax = v;
        if ( v < inputMin ) inputMin = v;
    }

    double inputOffset = ( inputMin < 128.0 ) ? inputMin : 0;
    double inputRange = inputMax - inputMin;
    if ( inputRange == 0.0 ) inputRange = 255.0;
    for ( int i = 0; i < INPUT_WIDTH * INPUT_HEIGHT; i++ ) {
        data[ i ] = ( inputData[ i ] - inputOffset ) / ( inputRange );
    }

    computeGradient( &data[ 0 ], INPUT_HEIGHT, INPUT_WIDTH, &gX[ 0 ], &gY[ 0 ] );
    edtaa3( &data[ 0 ], &gX[ 0 ], &gY[ 0 ], INPUT_HEIGHT, INPUT_WIDTH, &xDist[ 0 ], &yDist[ 0 ], &outside[ 0 ] );
    for ( int i = 0; i < INPUT_WIDTH * INPUT_HEIGHT; i++ ) {
        if ( outside[ i ] < 0.0 ) {
            outside[ i ] = 0.0;
        }
    }

    memset( &gX[ 0 ], 0, sizeof( double ) * INPUT_WIDTH * INPUT_HEIGHT );
    memset( &gY[ 0 ], 0, sizeof( double ) * INPUT_WIDTH * INPUT_HEIGHT );
    for ( int i = 0; i < INPUT_WIDTH * INPUT_HEIGHT; i++ ) {
        data[ i ] = 1 - data[ i ];
    }

    computeGradient( &data[ 0 ], INPUT_HEIGHT, INPUT_WIDTH, &gX[ 0 ], &gY[ 0 ] );
    edtaa3( &data[ 0 ], &gX[ 0 ], &gY[ 0 ], INPUT_HEIGHT, INPUT_WIDTH, &xDist[ 0 ], &yDist[ 0 ], &inside[ 0 ] );
    for ( int i = 0; i < INPUT_WIDTH * INPUT_HEIGHT; i++ ) {
        if ( inside[ i ] < 0.0 ) {
            inside[ i ] = 0.0;
        }
    }

    int channels = 3;
    std::vector< unsigned char > out( INPUT_WIDTH * INPUT_HEIGHT * channels * sizeof( unsigned char ) );
    double dist;
    for ( int i = 0; i < INPUT_WIDTH * INPUT_HEIGHT; i++ ) {
        dist = outside[ i ] - inside[ i ];
        dist = 128.0 + dist;
        if ( dist < 0.0 ) dist = 0.0;
        if ( dist >= 256.0 ) dist = 255.999;
        out[ 3 * i + 2 ] = ( unsigned char ) dist;
        out[ 3 * i + 1 ] = ( unsigned char ) ( ( dist - floor( dist ) ) * 256.0 );
        out[ 3 * i ] = inputData[ i ];
    }

	std::string outputPath = source.substr( 0, source.find_last_of( "." ) ) + "_sdf.tga";
	ImageTGA output;
	output.setData( input.getWidth(), input.getHeight(), channels, &out[ 0 ] );
	output.save( outputPath );

#ifdef __APPLE__
	system( ( std::string( "open \"" ) + source + "\"" ).c_str() );
	system( ( std::string( "open \"" ) + outputPath + "\"" ).c_str() );
#endif
}

void SDFGenerator::computeGradient( double *img, int w, int h, double *gx, double *gy )
{
    for ( int i = 1; i < h - 1; i++ ) {
        for ( int j = 1; j < w - 1; j++ ) {
            int k = i * w + j;
            if ( ( img[ k ] > 0.0 ) && ( img[ k ] < 1.0 ) ) {
                gx[ k ] = -img[ k - w - 1 ] - Numericd::SQRT_TWO * img[ k - 1 ] - img[ k + w - 1 ] + img[ k - w + 1 ] + Numericd::SQRT_TWO * img[ k + 1 ] + img[ k + w + 1 ];
                gy[ k ] = -img[ k - w - 1 ] - Numericd::SQRT_TWO * img[ k - w ] - img[ k + w - 1 ] + img[ k - w + 1 ] + Numericd::SQRT_TWO * img[ k + w ] + img[ k + w + 1 ];
                double gLength = gx[ k ] * gx[ k ] + gy[ k ] * gy[ k ];
                if ( gLength > 0.0 ) {
                    gLength = sqrt( gLength );
                    gx[ k ] = gx[ k ] / gLength;
                    gy[ k ] = gy[ k ] / gLength;
                }
            }
        }
    }
}

double SDFGenerator::edgedf( double gx, double gy, double a )
{
    double df, glength, temp, a1;

    if ( ( gx == 0 ) || ( gy == 0 ) ) {
        df = 0.5 - a;
    } else {
        glength = sqrt( gx * gx + gy * gy );
        if ( glength > 0 ) {
            gx = gx / glength;
            gy = gy / glength;
        }

        gx = fabs( gx );
        gy = fabs( gy );
        if ( gx < gy ) {
            temp = gx;
            gx = gy;
            gy = temp;
        }

        a1 = 0.5 * gy / gx;
        if ( a < a1 ) {
            df = 0.5 * ( gx + gy ) - sqrt( 2.0 * gx * gy * a );
        } else if ( a < ( 1.0 - a1 ) ) {
            df = ( 0.5 - a ) * gx;
        } else {
            df = -0.5 * ( gx + gy ) + sqrt( 2.0 * gx * gy * ( 1.0 - a ) );
        }
    }    

    return df;
}

double SDFGenerator::distaa3( double *img, double *gximg, double *gyimg, int w, int c, int xc, int yc, int xi, int yi )
{
  double di, df, dx, dy, gx, gy, a;
  int closest;
  
  closest = c-xc-yc*w;
  a = img[closest];
  gx = gximg[closest];
  gy = gyimg[closest];
  
  if(a > 1.0) a = 1.0;
  if(a < 0.0) a = 0.0;
  if(a == 0.0) return 1000000.0;

  dx = (double)xi;
  dy = (double)yi;
  di = sqrt(dx*dx + dy*dy);
  if(di==0) {
      df = edgedf(gx, gy, a);
  } else {
      df = edgedf(dx, dy, a);
  }
  return di + df;
}

void SDFGenerator::edtaa3(double *img, double *gx, double *gy, int w, int h, short *distx, short *disty, double *dist)
{
  int x, y, i, c;
  int offset_u, offset_ur, offset_r, offset_rd,
  offset_d, offset_dl, offset_l, offset_lu;
  double olddist, newdist;
  int cdistx, cdisty, newdistx, newdisty;
  int changed;
  double epsilon = 1e-3; // Safeguard against errors due to limited precision

  /* Initialize index offsets for the current image width */
  offset_u = -w;
  offset_ur = -w+1;
  offset_r = 1;
  offset_rd = w+1;
  offset_d = w;
  offset_dl = w-1;
  offset_l = -1;
  offset_lu = -w-1;

  /* Initialize the distance images */
  for(i=0; i<w*h; i++) {
    distx[i] = 0; // At first, all pixels point to
    disty[i] = 0; // themselves as the closest known.
    if(img[i] <= 0.0)
      {
	dist[i]= 1000000.0; // Big value, means "not set yet"
      }
    else if (img[i]<1.0) {
      dist[i] = edgedf(gx[i], gy[i], img[i]); // Gradient-assisted estimate
    }
    else {
      dist[i]= 0.0; // Inside the object
    }
  }

  /* Perform the transformation */
  do
    {
      changed = 0;

      /* Scan rows, except first row */
      for(y=1; y<h; y++)
        {

          /* move index to leftmost pixel of current row */
          i = y*w;

          /* scan right, propagate distances from above & left */

          /* Leftmost pixel is special, has no left neighbors */
          olddist = dist[i];
          if(olddist > 0) // If non-zero distance or not set yet
            {
	      c = i + offset_u; // Index of candidate for testing
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx;
              newdisty = cdisty+1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_ur;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx-1;
              newdisty = cdisty+1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  changed = 1;
                }
            }
          i++;

          /* Middle pixels have all neighbors */
          for(x=1; x<w-1; x++, i++)
            {
              olddist = dist[i];
              if(olddist <= 0) continue; // No need to update further

	      c = i+offset_l;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx+1;
              newdisty = cdisty;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_lu;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx+1;
              newdisty = cdisty+1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_u;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx;
              newdisty = cdisty+1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_ur;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx-1;
              newdisty = cdisty+1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  changed = 1;
                }
            }

          /* Rightmost pixel of row is special, has no right neighbors */
          olddist = dist[i];
          if(olddist > 0) // If not already zero distance
            {
	      c = i+offset_l;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx+1;
              newdisty = cdisty;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_lu;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx+1;
              newdisty = cdisty+1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_u;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx;
              newdisty = cdisty+1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  changed = 1;
                }
            }

          /* Move index to second rightmost pixel of current row. */
          /* Rightmost pixel is skipped, it has no right neighbor. */
          i = y*w + w-2;

          /* scan left, propagate distance from right */
          for(x=w-2; x>=0; x--, i--)
            {
              olddist = dist[i];
              if(olddist <= 0) continue; // Already zero distance

	      c = i+offset_r;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx-1;
              newdisty = cdisty;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  changed = 1;
                }
            }
        }
      
      /* Scan rows in reverse order, except last row */
      for(y=h-2; y>=0; y--)
        {
          /* move index to rightmost pixel of current row */
          i = y*w + w-1;

          /* Scan left, propagate distances from below & right */

          /* Rightmost pixel is special, has no right neighbors */
          olddist = dist[i];
          if(olddist > 0) // If not already zero distance
            {
	      c = i+offset_d;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx;
              newdisty = cdisty-1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_dl;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx+1;
              newdisty = cdisty-1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  changed = 1;
                }
            }
          i--;

          /* Middle pixels have all neighbors */
          for(x=w-2; x>0; x--, i--)
            {
              olddist = dist[i];
              if(olddist <= 0) continue; // Already zero distance

	      c = i+offset_r;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx-1;
              newdisty = cdisty;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_rd;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx-1;
              newdisty = cdisty-1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_d;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx;
              newdisty = cdisty-1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_dl;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx+1;
              newdisty = cdisty-1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  changed = 1;
                }
            }
          /* Leftmost pixel is special, has no left neighbors */
          olddist = dist[i];
          if(olddist > 0) // If not already zero distance
            {
	      c = i+offset_r;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx-1;
              newdisty = cdisty;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_rd;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx-1;
              newdisty = cdisty-1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  olddist=newdist;
                  changed = 1;
                }

	      c = i+offset_d;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx;
              newdisty = cdisty-1;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  changed = 1;
                }
            }

          /* Move index to second leftmost pixel of current row. */
          /* Leftmost pixel is skipped, it has no left neighbor. */
          i = y*w + 1;
          for(x=1; x<w; x++, i++)
            {
              /* scan right, propagate distance from left */
              olddist = dist[i];
              if(olddist <= 0) continue; // Already zero distance

	      c = i+offset_l;
	      cdistx = distx[c];
	      cdisty = disty[c];
              newdistx = cdistx+1;
              newdisty = cdisty;
              newdist = distaa3(img, gx, gy, w, c, cdistx, cdisty, newdistx, newdisty);
              if(newdist < olddist-epsilon)
                {
                  distx[i]=newdistx;
                  disty[i]=newdisty;
                  dist[i]=newdist;
                  changed = 1;
                }
            }
        }
    }
  while(changed); // Sweep until no more updates are made

  /* The transformation is completed. */

}

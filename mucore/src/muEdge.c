/* ------------------------------------------------------------------------- /
 *
 * Module: muEdge.c
 * Author: Nobel Hsu, Joe Lin
 *
 * Description:
 *    Gradients, Edges and Corners
 *
 -------------------------------------------------------------------------- */
#include "muCore.h"

/*===========================================================================================*/
/*   muLaplace                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a edge detection by Laplace operator.                             */
/*                                                                                           */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*   selection 1~2, two difference masks are able to use                                     */
// 1 = 0, 1, 0,      2 = 1, 1, 1,                                                            */
//     1,-4, 1,          1,-8, 1,                                                            */
//     0, 1, 0           1, 1, 1                                                             */
//                                                                                           */
/*===========================================================================================*/
muError_t muLaplace( const muImage_t* src, muImage_t* dst, MU_8U selection)
{
	MU_16S temp; 
	MU_32S i,j, index;
	MU_32S width, height;
	MU_8U *in, *out;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}


	width = src->width;
	height = src->height;

	in = src->imagedata;
	out= dst->imagedata;

	switch (selection)
	{
		case 1:

			for(j=0; j<(height-1); j++)
				for(i=0; i<(width-1); i++)
				{
					index = i+width*j;

					temp = abs((in[index+1]+in[index+width]+in[index+width+2]+in[index+(width<<1)+1])-(in[index+width+1]<<2));
					temp = temp > 255 ? 255 : temp;
					out[index+width+1]  = (MU_8U)temp;

				}
			break;

		case 2:

			for(j=0; j<(height-1); j++)
				for(i=0; i<(width-1); i++)
				{
					index = i+width*j;

					temp =  abs((in[index]+in[index+1]+in[index+2]+in[index+width]+in[index+width+2]+in[index+(width<<1)]+in[index+(width<<1)+1]
								+in[index+(width<<1)+2]) - (in[index+width+1]<<3));
					temp = temp > 255 ? 255 : temp;
					out[index+width+1] = (MU_8U)temp; 
				}
			break;
	}

	return MU_ERR_SUCCESS;
}


/*===========================================================================================*/
/*   muSobel                                                                                */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a edge detection by Sobel operator.                               */
/*                                                                                           */
/*   Gx = 1, 2, 1,    Gy = 1, 0, -1,    out = abs(Gx)+abs(Gy)                                */
/*        0, 0, 0,         2, 0, -2,                                                         */
/*       -1,-2,-1          1, 0, -1                                                          */ 
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/
muError_t muSobel( const muImage_t* src, muImage_t* dst)
{
	MU_16S temp; 
	MU_32S i,j, index;
	MU_32S gx,gy;
	MU_32S width, height;
	MU_8U *in, *out;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}


#if defined(HISI3516) || defined(HISI3531)
	if((src->depth & MU_IMG_HW_ACCE) && (dst->depth & MU_IMG_HW_ACCE))
	{
		platformSobel(src, dst);
		return MU_ERR_SUCCESS;
	}
#endif


	width = src->width;
	height = src->height;

	in = src->imagedata;
	out = dst->imagedata;

	for(j=0; j<(height-1); j++)
		for(i=0; i<(width-1); i++)
		{
			index = i+width*j;

			gx = (in[index]+(in[index+1]<<1)+(in[index+2]))-
				(in[index+(width<<1)]+(in[index+(width<<1)+1]<<1)+in[index+(width<<1)+2]);

			gy = (in[index]+(in[index+width]<<1)+in[index+(width<<1)])-
				(in[index+2]+(in[index+width+2]<<1)+in[index+(width<<1)+2]);

			temp = abs(gx)+abs(gy);

			temp = temp > 255 ? 255 : temp;
			temp = temp < 0 ? 0 : temp;

			out[index+width+1] = (MU_8U)temp;

		}

	return MU_ERR_SUCCESS;
}



/*===========================================================================================*/
/*   muPrewitt                                                                              */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a edge detection by Prewitt operator.                             */
/*                                                                                           */
/*   Gx = 1, 1, 1,    Gy = 1, 0, -1,    out = abs(Gx)+abs(Gy)                                */
/*        0, 0, 0,         1, 0, -1,                                                         */
/*       -1,-1,-1          1, 0, -1                                                          */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*                                                                                           */
/*===========================================================================================*/
muError_t muPrewitt( const muImage_t* src, muImage_t* dst)
{
	MU_16S temp; 
	MU_32S i,j, index;
	MU_32S gx,gy;
	MU_32S width, height;
	MU_8U *in, *out;
	muError_t ret;

	ret = muCheckDepth(4, src, MU_IMG_DEPTH_8U, dst, MU_IMG_DEPTH_8U);
	if(ret)
	{
		return ret;
	}

	if(src->channels != 1 || dst->channels != 1)
	{
		return MU_ERR_NOT_SUPPORT;
	}


	width = src->width;
	height = src->height;

	in = src->imagedata;
	out = dst->imagedata;

	for(j=0; j<(height-1); j++)
		for(i=0; i<(width-1); i++)
		{
			index = i+width*j;

			gx = (in[index]+(in[index+1])+(in[index+2]))-
				(in[index+(width<<1)]+(in[index+(width<<1)+1])+in[index+(width<<1)+2]);

			gy = (in[index]+(in[index+width])+in[index+(width<<1)])-
				(in[index+2]+(in[index+width+2])+in[index+(width<<1)+2]);

			temp = abs(gx)+abs(gy);

			temp = temp > 255 ? 255 : temp;
			temp = temp < 0 ? 0 : temp;

			out[index+width+1] = (MU_8U)temp;

		}

	return MU_ERR_SUCCESS;
}


/*===========================================================================================*/
/*   muCanny                                                                                */
/*                                                                                           */
/*   DESCRIPTION:                                                                            */
/*   This routine performs a edge detection by Prewitt operator.                             */
/*                                                                                           */
/*   Gx = 1, 1, 1,    Gy = 1, 0, -1,    out = abs(Gx)+abs(Gy)                                */
/*        0, 0, 0,         1, 0, -1,                                                         */
/*       -1,-1,-1          1, 0, -1                                                          */   
/*                                                                                           */
/*   NOTE                                                                                    */
/*                                                                                           */
/*   USAGE                                                                                   */
/*   muImage_t *src --> input image                                                           */
/*   muImage_t *dst --> output image                                                          */
/*   muImage_t *ang --> direction image                                                       */
/*===========================================================================================*/
muError_t muCanny( const muImage_t* src, muImage_t* dst, muImage_t *ang)
{
/*
	if(src->depth != MU_IMG_DEPTH_8U ||
			dst->depth != MU_IMG_DEPTH_8U) 
	{
		return MU_ERR_NOT_SUPPORT; 
	}
*/
	return MU_ERR_SUCCESS;
}


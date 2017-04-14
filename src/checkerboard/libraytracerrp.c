#include "libraytracerrp.h"

//actual ray tracing
double trace_ray(double lat1, double long1, double lat2, double long2,
		 int xNode, int yNode, double **dispersionData,
		 int nParallelMeasurements, int nPerpendicularMeasurements,
		 double latMin, double latInc, double longMin, double longInc,
		 double widthDeg, double aveVel)
{
		
	/* usage parameters:
	 * lat1, long1, lat2, long2 - the begin (1) and end (2) points of the ray
	 * xNode, yNode, dispersionData - dispersionData is an xNode x yNode matrix with the data
	 * nParallelMeasurements, nPerpendicularMeasurements - number of values to compute along the raypath
	 * latMin, latInc, longMin, longInc - minimum values and increments used in the dispersionData
	 * widthDeg - the width in km of a single ray
	 * aveVel - the average velocity in the dispersionData (should be slightly faster when this routine is used many times to pass it rather than recompute)
	 * value returned is the average velocity along the ray path
	 */
	int i, l, x, y, x2, y2;
	int hitCount=0;
	DISTAZ distanceAndAzimuth;
	double startLat, startLong, endLat, endLong, endLat2, endLong2;
	double xOrig,   yOrig,   zOrig;
	double xTarget, yTarget, zTarget;
	double xStep,   yStep,   zStep;
	double distance, azimuth, deltaDeg;
	double tmpLong, tmpLat, dx, dy;
	double vel = 0.0, vel1=0.0, vel2=0.0;
	double computedTime = 0.0;
	double tmpAveVel = 0.0;
	/* for debugging. see commented out lines below */
	DISTAZ tmpDistAz;
	double currentWidth;
//	double tmpdist=0.0;
	double prevLat, prevLong;
	double accumulatedDistance=0.0;
//	char tmpFileName[100];
//	FILE *tmpFile;
//	sprintf(tmpFileName,"RayPath_%3.1lf_%3.1lf_%3.1lf_%3.1lf",lat1,long1,lat2,long2);
//	tmpFile = fopen(tmpFileName,"w");

	/* compute the total distance */
	distanceAndAzimuth = distaz(lat1, long1, lat2, long2);
	distance = distanceAndAzimuth.distance;
	azimuth = distanceAndAzimuth.azimuth;

	//converting to cartesian
	startLat = lat1;
	startLong = long1;
	endLat = lat2;
	endLong = long2;

	//the first point
	xOrig = EARTH_RADIUS * cos(startLat * PI / 180.0) * cos(startLong * PI / 180.0);
	yOrig = EARTH_RADIUS * cos(startLat * PI / 180.0) * sin(startLong * PI / 180.0);
	zOrig = EARTH_RADIUS * sin(startLat * PI / 180.0);

	//the end point
	xTarget = EARTH_RADIUS * cos(endLat * PI / 180.0) * cos(endLong * PI / 180.0);
	yTarget = EARTH_RADIUS * cos(endLat * PI / 180.0) * sin(endLong * PI / 180.0);
	zTarget = EARTH_RADIUS * sin(endLat * PI / 180.0);

	//the distance in each step
	deltaDeg = sqrtf(powf(xOrig - xTarget,2) + powf(yOrig - yTarget, 2) + powf(zOrig - zTarget,2))/nParallelMeasurements; 

	//the initial point is kept for distance calculation
	prevLat = startLat;
	prevLong = startLong;

	//breaking up along a path
	for (i=-1; i<nParallelMeasurements+1; i++) {
		vel = 0.0;	
		hitCount = 0;
		xStep = xOrig + (i+.5) * (xTarget - xOrig) / nParallelMeasurements;
		yStep = yOrig + (i+.5) * (yTarget - yOrig) / nParallelMeasurements;
		zStep = zOrig + (i+.5) * (zTarget - zOrig) / nParallelMeasurements;
		
		//convert back to sphere
	        tmpLong = 180.0 / PI * atan2(yStep,xStep);
	        tmpLat = 90 - 180.0 / PI * acos(zStep / sqrtf(xStep*xStep + yStep * yStep + zStep * zStep));  //assumes colatitude in the northern hemisphere

		//slight adjustment for the start point
		if (i==-1) {
			tmpLong = long1;
			tmpLat = lat1;
		}

		//slight adjustment for the end point
		if (i == nParallelMeasurements) {	
			tmpLong = long2;
			tmpLat = lat2;
		}

		//computing the distance for the given segment
		tmpDistAz = distaz(prevLat,prevLong,tmpLat,tmpLong);
		accumulatedDistance = accumulatedDistance + tmpDistAz.distance;
//		printf("tmpDistAz.distance: %lf\n",tmpDistAz.distance);
//		if (i == nParallelMeasurements-1) {
//			printf("start point: %lf %lf. tmp: %lf %lf. prev: %lf %lf. end point: %lf %lf\n",startLat, startLong, tmpLat, tmpLong, prevLat, prevLong, lat2, long2);
//		}

		//keeping the current values for computation of distance in next step
		prevLat = tmpLat;
		prevLong = tmpLong;

		//apply a sine wave function for the ray thickness
		currentWidth = widthDeg * (PI * accumulatedDistance / distance);

		//get points along a wavelength
		for (l=0; l<nPerpendicularMeasurements+1; l++) {
			// compute the deviation from the start point 
			dx = (currentWidth / 2.0 - l*currentWidth/((double) nPerpendicularMeasurements)) * sin( ( azimuth - 90.0) * PI / 180);
		        dy = (currentWidth / 2.0 - l*currentWidth/((double) nPerpendicularMeasurements)) * cos( ( azimuth - 90.0) * PI / 180);

		        //apply the perturbation 
		        endLong = tmpLong + dx;
		        endLat = tmpLat + dy;
                        endLong2 = tmpLong - dx;
                        endLat2 = tmpLat - dy;

                        //Find which voxel this perturbed point is inside
                        x = floor((endLong - longMin) / longInc+0.5);
                        y = floor((endLat - latMin) / latInc+0.5);
                        x2 = floor((endLong2 - longMin) / longInc+0.5);
                        y2 = floor((endLat2 - latMin) / latInc+0.5);
//			fprintf(tmpFile,"%lf %lf %lf %lf %d %d %d %d\n",endLong,endLat,endLong2,endLat2,x,y,x2,y2);

/*
                                if (x < 0 || x >= xNode) {
                                        vel = vel + aveVel;
                                } else if (y < 0 || y >= yNode) {
                                        vel = vel + aveVel;
                                } else {
                                        //add the contribution of this point
                                        vel = vel + dispersionMapData[x][y];
                                }
*/
                        if (x < 0 || x >= xNode || y < 0 || y >= yNode) {
                        	vel1 = aveVel;
                        } else {
                        	vel1 = dispersionData[x][y];
                        }
                        if (x2 < 0 || x2 >= xNode || y2 < 0 || y2 >= yNode) {
                        	vel2 = aveVel;
                        } else {
                        	vel2 = dispersionData[x2][y2];
                        }


                        vel = vel + (vel1 + vel2) / 2.0;
//			vel = vel + vel1;
			hitCount++;
		}
		
		//compute the time spent along the perpendicular line
		tmpAveVel = vel / (double) hitCount;
		computedTime = computedTime + (tmpDistAz.distance * 1.) / tmpAveVel;
	}
//	printf("Distance: %lf. Accumulated Distance: %lf\n",distance,accumulatedDistance);
//	fclose(tmpFile);
	return distance / computedTime;
}






double recursive_mean(double new_value, double old_mean, int n) {
        return (1 / (double) n) * (( (double) n-1) * old_mean + new_value);
}
//END

double powi(double b, long x) {
        double temp=0.0;
        long i;

        if ( b == 0.0 ) {
                return( (double) 0.0 );
        }
        if ( x == 0L ) {
                return( (double) 1.0 );
        }

    if ( x > 0L ) {
                temp = b;
        for ( i = x-1; i > 0L; i-- ) {
                temp *= b;
        }
        return temp;
        }

    if ( x < 0L ) {
        temp = 1.0 / b;
        for ( i = x+1; i < 0L; i++ ) {
                temp *= (1.0/b);
        }
        return temp;
        }
        return temp;
}
///END

DISTAZ distaz(double lat1, double long1, double lat2, double long2)
{
	int laz=1, ldist=1, lxdeg=1;
//	int idx, ns=1;
	double a, a1, a12, a12bot, a12top, al, b, b0, b1, c, c0, c1, c2, 
	 c4, cosa12, costhi, costhk, d, d1, dl, du, e, e1, e1p1, e2, e3, 
	 ec2, el, eps, f, f1, g, g1, h, h1, onemec2, p1, p2, pdist, pherad, 
	 phsrad, sc, sd, sina12, sinthi, sinthk, sqrte1p1, ss, t1, t2, 
	 tanthi, tanthk, temp, therad, thg, thsrad, u1, u1bot, u2, u2bot, 
	 u2top, v1, v2, x2, y2, z1, z2;
	
	double the=lat1, phe=long1, ths=lat2, phs=long2;
	double xdeg, azi, distance;

	DISTAZ out;

	static double rad = 6378.160;  /* Earth Radius */
	static double fl = 0.00335293; /* Earth Flattening */
	static double twopideg = 360.; /* Two Pi in Degrees */
	static double c00 = 1.;
	static double c01 = 0.25;
	static double c02 = -4.6875e-02;
	static double c03 = 1.953125e-02;
	static double c21 = -0.125;
	static double c22 = 3.125e-02;
	static double c23 = -1.46484375e-02;
	static double c42 = -3.90625e-03;
	static double c43 = 2.9296875e-03;
//	static double degtokm = 111.3199;  /* Conversion from degrees to km */


//	double *const Az = &az[0] - 1;
//	double *const Dist = &dist[0] - 1;
//	double *const Phs = &phs - 1;
//	double *const Ths = &ths - 1;
//	double *const Xdeg = &xdeg[0] - 1;


	/* - Initialize. */
	ec2 = 2.*fl - fl*fl;
	onemec2 = 1. - ec2;
	eps = 1. + ec2/onemec2;


	/* - Convert event location to radians.
	 *   (Equations are unstable for latidudes of exactly 0 degrees.) */
	temp = the;
	if( temp == 0. ) {
	    temp = 1.0e-08;
	}
        therad = TORAD*temp;
        pherad = TORAD*phe;
       

	/* - Must convert from geographic to geocentric coordinates in order
	 *   to use the spherical trig equations.  This requires a latitude
	 *   correction given by: 1-EC2=1-2*FL+FL*FL */
	if ( the == 90 || the == -90 ) {/* special attention at the poles */
	    thg = the*TORAD ;		/* ... to avoid division by zero. */
        } else {
            thg = atan( onemec2 * tan(therad) );
        }
	d = sin( pherad );
	e = -cos( pherad );
	f = -cos( thg );
	c = sin( thg );
	a = f*e;
	b = -f*d;
	g = -c*e;
	h = c*d;

	    /* -- Convert to radians. */
	    temp = ths;
	    if( temp == 0. )
		temp = 1.0e-08;
	    thsrad = TORAD*temp;
	    phsrad = TORAD*phs;

	    /* -- Calculate some trig constants. */
	    if ( ths == 90 || ths == -90 )
		thg = ths * TORAD ;
	    else
		thg = atan( onemec2*tan( thsrad ) );
	    d1 = sin( phsrad );
	    e1 = -cos( phsrad );
	    f1 = -cos( thg );
	    c1 = sin( thg );
	    a1 = f1*e1;
	    b1 = -f1*d1;
	    g1 = -c1*e1;
	    h1 = c1*d1;
	    sc = a*a1 + b*b1 + c*c1;

	    /* - Spherical trig relationships used to compute angles. */

	    if( lxdeg ){
		sd = 0.5*sqrt( (powi(a - a1,2) + powi(b - b1,2) + powi(c - 
		 c1,2))*(powi(a + a1,2) + powi(b + b1,2) + powi(c + c1,2)) );
		xdeg = atan2( sd, sc )*TODEG;
		if( xdeg < 0. ) {
		    xdeg = xdeg + twopideg;
		}
	    }

	    if( laz ){
		ss = powi(a1 - d,2) + powi(b1 - e,2) + powi(c1,2) - 2.;
		sc = powi(a1 - g,2) + powi(b1 - h,2) + powi(c1 - f,2) - 2.;
		azi = atan2( ss, sc )*TODEG;
		if( azi < 0. ) {
		    azi = azi + twopideg;
		}
	    }
	    /* - Now compute the distance between the two points using Rudoe's
	     *   formula given in GEODESY, section 2.15(b).
	     *   (There is some numerical problem with the following formulae.
	     *   If the station is in the southern hemisphere and the event in
	     *   in the northern, these equations give the longer, not the
	     *   shorter distance between the two locations.  Since the
	     *   equations are fairly messy, the simplist solution is to reverse
	     *   the meanings of the two locations for this case.) */
	    if( ldist ){
		if( thsrad > 0. ){
		    t1 = thsrad;
		    p1 = phsrad;
		    t2 = therad;
		    p2 = pherad;

		    /* special attention at the poles to avoid atan2 troubles 
		       and division by zero. */
                    if ( the == 90.0 ) {
			costhk = 0.0 ;
			sinthk = 1.0 ;
			tanthk = FLT_MAX ;
		    }
                    else if ( the == -90.0 ) {
			costhk = 0.0 ;
			sinthk = -1.0 ;
			tanthk = -FLT_MAX ;
		    }
                    else {
			costhk = cos( t2 );
			sinthk = sin( t2 );
			tanthk = sinthk/costhk;
                    }

		    /* special attention at the poles continued. */
		    if ( ths == 90.0 ) {
                        costhi = 0.0 ;
                        sinthi = 1.0 ;
                        tanthi = FLT_MAX ;
                    }
		    else if ( ths == -90.0 ) {
                        costhi = 0.0 ;
                        sinthi = -1.0 ;
                        tanthi = -FLT_MAX ;
		    }
		    else {
			costhi = cos( t1 );
			sinthi = sin( t1 );
			tanthi = sinthi/costhi;
		    } 
		}
		else{
		    t1 = therad;
		    p1 = pherad;
		    t2 = thsrad;
		    p2 = phsrad;

		    /* more special attention at the poles */
                    if ( ths == 90.0 ) {
                        costhk = 0.0 ;
                        sinthk = 1.0 ;
                        tanthk = FLT_MAX ;
                    }
                    else if ( ths == -90.0 ) {
                        costhk = 0.0 ;
                        sinthk = -1.0 ;
                        tanthk = -FLT_MAX ;
                    }
		    else {
                        costhk = cos( t2 );
                        sinthk = sin( t2 );
                        tanthk = sinthk/costhk;
                    } 

		    /* more special attention at the poles continued */
                    if ( the == 90.0 ) {
                        costhi = 0.0 ;
                        sinthi = 1.0 ;
                        tanthi = FLT_MAX ;
                    }
                    else if ( the == -90.0 ) {
                        costhi = 0.0 ;
                        sinthi = -1.0 ;
                        tanthi = -FLT_MAX ;
                    }
		    else {
                        costhi = cos( t1 );
                        sinthi = sin( t1 );
                        tanthi = sinthi/costhi;
                    }

		}

		el = ec2/onemec2;
		e1 = 1. + el;
		al = tanthi/(e1*tanthk) + ec2*sqrt( (e1 + powi(tanthi,2))/
		 (e1 + powi(tanthk,2)) );
		dl = p1 - p2;
		a12top = sin( dl );
		a12bot = (al - cos( dl ))*sinthk;

		/* Rewrote these three lines with help from trig identities.  maf 990415 */
		a12 = atan2( a12top, a12bot );
		cosa12 = cos( a12 );
		sina12 = sin( a12 );

		/*cosa12 = sqrt ( a12bot*a12bot / ( a12bot*a12bot + a12top*a12top ) ) ;
		sina12 = sqrt ( a12top*a12top / ( a12bot*a12bot + a12top*a12top ) ) ; */

		e1 = el*(powi(costhk*cosa12,2) + powi(sinthk,2));
		e2 = e1*e1;
		e3 = e1*e2;
		c0 = c00 + c01*e1 + c02*e2 + c03*e3;
		c2 = c21*e1 + c22*e2 + c23*e3;
		c4 = c42*e2 + c43*e3;
		v1 = rad/sqrt( 1. - ec2*powi(sinthk,2) );
		v2 = rad/sqrt( 1. - ec2*powi(sinthi,2) );
		z1 = v1*(1. - ec2)*sinthk;
		z2 = v2*(1. - ec2)*sinthi;
		x2 = v2*costhi*cos( dl );
		y2 = v2*costhi*sin( dl );
		e1p1 = e1 + 1.;
		sqrte1p1 = sqrt( e1p1 );
		u1bot = sqrte1p1*cosa12;
		u1 = atan2( tanthk, u1bot );
		u2top = v1*sinthk + e1p1*(z2 - z1);
		u2bot = sqrte1p1*(x2*cosa12 - y2*sinthk*sina12);
		u2 = atan2( u2top, u2bot );
		b0 = v1*sqrt( 1. + el*powi(costhk*cosa12,2) )/e1p1;
		du = u2 - u1;
		pdist = b0*(c2*(sin( 2.*u2 ) - sin( 2.*u1 )) + c4*(sin( 4.*
		 u2 ) - sin( 4.*u1 )));
		distance = fabs( b0*c0*du + pdist );
	    } /* end if ( ldist ) */

	out.distance = distance;
	out.azimuth = azi;
	out.degrees = xdeg;

	return out;
}



int compute_nodes(double max, double min, double inc) {
	return floor( ( max - min) / inc + 1.5);
}
//compute the distance in degrees
double degree_dist(double lat1, double long1, double lat2, double long2) {
        return acos(sin(lat1 * PI / 180.0) * sin(lat2 * PI / 180.0) + cos(lat1 *PI / 180.0) *cos(lat2 * PI / 180.0) * cos((long2 - long1) * PI / 180.0)) * 180.0 / PI;
}
//END
//compute the distance in kilometers
double km_dist(double lat1, double long1, double lat2, double long2) {
        return degree_dist(lat1,long1,lat2,long2) * PI / 180.0 * EARTH_RADIUS;
}
//END
//compute the event-station azimuth
double compute_event_station_azimuth(double lat1, double long1, double lat2, double long2) {
        double colatB, inter_long;
        colatB = (90.0 - lat2) * PI / 180.0;
        inter_long = (long1 - long2) * PI / 180.0;
        return (inter_long * colatB / (degree_dist(lat1, long1, lat2, long2) * PI / 180.0)) * 180.0 / PI;
}
//END

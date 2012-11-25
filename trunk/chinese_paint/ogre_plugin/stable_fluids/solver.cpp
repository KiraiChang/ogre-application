#define IX(i,j) ((i)+(N+2)*(j))
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
#define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) {
#define END_FOR }}

#include "stroke\point.h"

void add_source ( int N, float * x, float * s, float dt )
{
	int i, size=(N+2)*(N+2);
	for ( i=0 ; i<size ; i++ ) x[i] += dt*s[i];
}

void set_bnd ( int N, int b, float * x, float * blocked )
{
	int i, j;

	//for ( i=1 ; i<=N ; i++ ) {
	//	x[IX(0  ,i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)];
	//	x[IX(N+1,i)] = b==1 ? -x[IX(N,i)] : x[IX(N,i)];
	//	x[IX(i,0  )] = b==2 ? -x[IX(i,1)] : x[IX(i,1)];
	//	x[IX(i,N+1)] = b==2 ? -x[IX(i,N)] : x[IX(i,N)];
	//}
	//x[IX(0  ,0  )] = 0.5f*(x[IX(1,0  )]+x[IX(0  ,1)]);
	//x[IX(0  ,N+1)] = 0.5f*(x[IX(1,N+1)]+x[IX(0  ,N)]);
	//x[IX(N+1,0  )] = 0.5f*(x[IX(N,0  )]+x[IX(N+1,1)]);
	//x[IX(N+1,N+1)] = 0.5f*(x[IX(N,N+1)]+x[IX(N+1,N)]);

	//for ( i=1 ; i<=N ; i++ ) {
	//	x[IX(0  ,i)] = b==1 ? 0 : x[IX(0,i)];
	//	x[IX(N+1,i)] = b==1 ? 0 : x[IX(N+1,i)];
	//	x[IX(i,0  )] = b==2 ? 0 : x[IX(i,0)];
	//	x[IX(i,N+1)] = b==2 ? 0 : x[IX(i,N+1)];
	//}
	//x[IX(0  ,0  )] = 0;
	//x[IX(0  ,N+1)] = 0;
	//x[IX(N+1,0  )] = 0;
	//x[IX(N+1,N+1)] = 0;

	for ( i=1 ; i<=N ; i++ ) 
	{
		x[IX(0  ,i)] = x[IX(1,i)];   // Left
		x[IX(N+1,i)] = x[IX(N,i)];   // Right
		x[IX(i,0  )] = x[IX(i,1)];   // Up
		x[IX(i,N+1)] = x[IX(i,N)];   // Down
	}
	x[IX(0  ,0  )] = 0.5f*(x[IX(1,0  )]+x[IX(0  ,1)]);
	x[IX(0  ,N+1)] = 0.5f*(x[IX(1,N+1)]+x[IX(0  ,N)]);
	x[IX(N+1,0  )] = 0.5f*(x[IX(N,0  )]+x[IX(N+1,1)]);
	x[IX(N+1,N+1)] = 0.5f*(x[IX(N,N+1)]+x[IX(N+1,N)]);

	if(blocked == 0)
		return;
	FOR_EACH_CELL
		if(!blocked[IX(i,j)]) continue;
	if( (b==1) || (b==2) ) {
		x[IX(i,j)]=0;
	}
	else {
		int count  =0;
		float total=0;
		if(!blocked[IX(i+1,j)]) { count++; total+=x[IX(i+1,j)]; }
		if(!blocked[IX(i-1,j)]) { count++; total+=x[IX(i-1,j)]; }		
		if(!blocked[IX(i,j+1)]) { count++; total+=x[IX(i,j+1)]; }
		if(!blocked[IX(i,j-1)]) { count++; total+=x[IX(i,j-1)]; }
		if(count) total/=count;
		x[IX(i,j)]=total;
	}
	END_FOR
}

void lin_solve ( int N, int b, float * x, float * x0, float a, float c, float * blocked  )
{
	int i, j, k;

	for ( k=0 ; k<20 ; k++ ) {
		FOR_EACH_CELL
			x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/c;
		END_FOR
		set_bnd ( N, b, x, blocked);
	}
}

void diffuse ( int N, int b, float * x, float * x0, float diff, float dt, float * blocked  )
{
	float a=dt*diff*N*N;
	lin_solve ( N, b, x, x0, a, 1+4*a, blocked );
}

void advect ( int N, int b, float * d, float * d0, float * u, float * v, float dt, float * blocked )
{
	//int i, j, i0, j0, i1, j1;
	//float x, y, s0, t0, s1, t1, dt0;

	//dt0 = dt*N;
	//FOR_EACH_CELL
	//	x = i-dt0*u[IX(i,j)]; y = j-dt0*v[IX(i,j)];
	//	if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; i0=(int)x; i1=i0+1;
	//	if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; j0=(int)y; j1=j0+1;
	//	s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
	//	d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+
	//				 s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
	//END_FOR
	//set_bnd ( N, b, d );

	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0, vx, vy, tleft,t,tnext,distx,disty;

	dt0 = dt*N;


	FOR_EACH_CELL

		if(blocked[IX(i,j)]) continue;

	tleft=dt0;
	x=i;y=j;		
	const float smallf = 0.0000001f;
	while(tleft>smallf) {

		//enforce boundry contraints
		if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; 
		if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; 


		i0=(int)x; i1=i0+1;
		j0=(int)y; j1=j0+1;
		s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;

		vx = -(s0*(t0*u[IX(i0,j0)]+t1*u[IX(i0,j1)])+
			s1*(t0*u[IX(i1,j0)]+t1*u[IX(i1,j1)]));

		vy = -(s0*(t0*v[IX(i0,j0)]+t1*v[IX(i0,j1)])+
			s1*(t0*v[IX(i1,j0)]+t1*v[IX(i1,j1)]));


		float speed2=vx*vx+vy*vy; 
		if(speed2>smallf) tnext=.5/sqrt(speed2);
		else tnext=tleft;

		t=tnext > tleft ? tleft : tnext;
		tleft-=t;


		x+=t*vx;
		y+=t*vy;
	}


	if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; 
	if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; 


	i0=(int)x; i1=i0+1;
	j0=(int)y; j1=j0+1;
	s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;

	d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+
		s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
	END_FOR
		set_bnd ( N, b, d, blocked );
}

void project ( int N, float * u, float * v, float * p, float * div, float * blocked  )
{
	int i, j;

	FOR_EACH_CELL
		div[IX(i,j)] = -0.5f*(u[IX(i+1,j)]-u[IX(i-1,j)]+v[IX(i,j+1)]-v[IX(i,j-1)])/N;
		p[IX(i,j)] = 0;
	END_FOR	
	set_bnd ( N, 0, div, blocked ); set_bnd ( N, 0, p, blocked );

	lin_solve ( N, 0, p, div, 1, 4, blocked );

	FOR_EACH_CELL
		u[IX(i,j)] -= 0.5f*N*(p[IX(i+1,j)]-p[IX(i-1,j)]);
		v[IX(i,j)] -= 0.5f*N*(p[IX(i,j+1)]-p[IX(i,j-1)]);
	END_FOR
	set_bnd ( N, 1, u, blocked ); set_bnd ( N, 2, v, blocked );
}

void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt, float * blocked  )
{
	add_source ( N, x, x0, dt );
	SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt, blocked );
	SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt, blocked );
}

void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt, float * blocked  )
{
	add_source ( N, u, u0, dt ); add_source ( N, v, v0, dt );
	SWAP ( u0, u ); diffuse ( N, 1, u, u0, visc, dt, blocked );
	SWAP ( v0, v ); diffuse ( N, 2, v, v0, visc, dt, blocked );
	project ( N, u, v, u0, v0, blocked );
	SWAP ( u0, u ); SWAP ( v0, v );
	advect ( N, 1, u, u0, u0, v0, dt, blocked ); advect ( N, 2, v, v0, u0, v0, dt, blocked );
	project ( N, u, v, u0, v0, blocked );
}

void wave_step ( int N, float * buf, float * buf1, float * buf2, float *dampening, float timePass, float **velocity )
{
	int x, y;
	//float height_x1y1;
	//float height_x0y1, height_x2y1;
	//float height_x1y0, height_x1y2;
	//float pre, dum;
	float dt = timePass * timePass;
	for(y=1;y<N;y++)
	{ 
		float *row = buf + y*(N) ;
		float *u = velocity[0] + y*(N) ;
		float *v = velocity[1] + y*(N) ;
		float *prevU = velocity[2] + y*(N) ;
		float *prevV = velocity[3] + y*(N) ;
		float *row1 = buf1 + y*(N) ;
		float *row1up = buf1 + (y-1)*(N) ;
		float *row1down = buf1 + (y+1)*(N) ;
		float *row2 = buf2 + y*(N) ;
		float *dump = dampening + y*(N) ;
		for(x=1;x<N;x++) 
		{
			//height_x1y1 = row1[x];
			//pre = row2[x];
			//height_x0y1 = row1[x-1];
			//height_x2y1 = row1[x+1];
			//height_x1y0 = row1up[x];
			//height_x1y2 = row1down[x];
			//dum = dump[x] ;
			//float force = 2 * dump[x] *  (row1[x-1] + row1[x+1] + row1up[x]+row1down[x] - (4 * row[x]));
			prevU[x] = u[x];prevV[x] = v[x];
			u[x] = 10 * /*dump[x] **/  (row1[x-1] + row1[x+1] - (2 * row[x])) * dt;
			v[x] = 10 * /*dump[x] **/  (row1up[x]+row1down[x] - (2 * row[x])) * dt;
			float force = u[x] + v[x];
			float newHight =  (1.99 * row1[x]) - (0.99 * row2[x]) + 0.1 * force;
			row[x] = newHight;
		}
	}
}

void particle_move(Stroke::LIST_POINT &vControlPoint, float **field, unsigned int N, float moveSpeed)
{

	Stroke::LIST_POINT::iterator ite;
	unsigned int index, index_right, index_down;
	float u0, u1, v0, v1, u, v;
	int x0, y0;//, x1, y1;

	for(ite = vControlPoint.begin(); ite != vControlPoint.end();++ite)
	{
		x0 = (int)ite->x; y0 = (int)ite->y;
		index = IX(x0,y0);
		index_right = index + 1;
		index_down = index + N;
		//x1 = x0 + 1; y1 = y0 + 1;
		u1 = ite->x-x0; u0 = 1-u1; v1 = ite->y-y0; v0 = 1-v1;
		u = u0*(v0*field[0][index]+v1*field[0][index_down])+
			u1*(v0*field[0][index_right]+v1*field[0][index_right]);

		v = u0*(v0*field[1][index]+v1*field[1][index_down])+
			u1*(v0*field[1][index_right]+v1*field[1][index_right]);

		ite->x += u * moveSpeed; ite->y += v * moveSpeed;
	}
}

void draw_line(Stroke::V_POINT &vControlPoint, float *grid, unsigned int N, float value, bool absolute)
{
	unsigned int i, count;
	int x, y;
	Stroke::Point p1, p2, dist;
	count = vControlPoint.size();
	for(i = 0; i < count; i++)
	{
		p1 = vControlPoint[i];
		p2 = vControlPoint[(i+1)%count];
		x = (int)p1.x;
		y = (int)p1.y;
		//printf("x:%d, y:%d, density:%f", x, y, grid[IX(x,y)]);
		if(absolute)
			grid[IX(x,y)] = value;
		else
			grid[IX(x,y)] += value;
		while(p2.distance(p1) > 1.0)
		{
			dist = (p2 - p1).normalisedCopy();
			p1 += dist;
			x = (int)p1.x;
			y = (int)p1.y;
			//printf("x:%d, y:%d, density:%f", x, y, grid[IX(x,y)]);
			if(absolute)
				grid[IX(x,y)] = value;
			else
				grid[IX(x,y)] += value;
		}
		x = (int)p2.x;
		y = (int)p2.y;
		//printf("x:%d, y:%d, density:%f\n", x, y, grid[IX(x,y)]);
		if(absolute)
			grid[IX(x,y)] = value;
		else
			grid[IX(x,y)] += value;
	}
}
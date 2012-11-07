#define IX(i,j) ((i)+(N+2)*(j))
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
#define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) {
#define END_FOR }}

void add_source ( int N, float * x, float * s, float dt )
{
	int i, size=(N+2)*(N+2);
	for ( i=0 ; i<size ; i++ ) x[i] += dt*s[i];
}

void set_bnd ( int N, int b, float * x )
{
	int i;

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

	for ( i=1 ; i<=N ; i++ ) {
		x[IX(0  ,i)] = b==1 ? 0 : x[IX(0,i)];
		x[IX(N+1,i)] = b==1 ? 0 : x[IX(N+1,i)];
		x[IX(i,0  )] = b==2 ? 0 : x[IX(i,0)];
		x[IX(i,N+1)] = b==2 ? 0 : x[IX(i,N+1)];
	}
	x[IX(0  ,0  )] = 0;
	x[IX(0  ,N+1)] = 0;
	x[IX(N+1,0  )] = 0;
	x[IX(N+1,N+1)] = 0;
}

void lin_solve ( int N, int b, float * x, float * x0, float a, float c )
{
	int i, j, k;

	for ( k=0 ; k<20 ; k++ ) {
		FOR_EACH_CELL
			x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/c;
		END_FOR
		set_bnd ( N, b, x );
	}
}

void diffuse ( int N, int b, float * x, float * x0, float diff, float dt )
{
	float a=dt*diff*N*N;
	lin_solve ( N, b, x, x0, a, 1+4*a );
}

void advect ( int N, int b, float * d, float * d0, float * u, float * v, float dt )
{
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;

	dt0 = dt*N;
	FOR_EACH_CELL
		x = i-dt0*u[IX(i,j)]; y = j-dt0*v[IX(i,j)];
		if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; i0=(int)x; i1=i0+1;
		if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; j0=(int)y; j1=j0+1;
		s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
		d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+
					 s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
	END_FOR
	set_bnd ( N, b, d );
}

void project ( int N, float * u, float * v, float * p, float * div )
{
	int i, j;

	FOR_EACH_CELL
		div[IX(i,j)] = -0.5f*(u[IX(i+1,j)]-u[IX(i-1,j)]+v[IX(i,j+1)]-v[IX(i,j-1)])/N;
		p[IX(i,j)] = 0;
	END_FOR	
	set_bnd ( N, 0, div ); set_bnd ( N, 0, p );

	lin_solve ( N, 0, p, div, 1, 4 );

	FOR_EACH_CELL
		u[IX(i,j)] -= 0.5f*N*(p[IX(i+1,j)]-p[IX(i-1,j)]);
		v[IX(i,j)] -= 0.5f*N*(p[IX(i,j+1)]-p[IX(i,j-1)]);
	END_FOR
	set_bnd ( N, 1, u ); set_bnd ( N, 2, v );
}

void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt )
{
	add_source ( N, x, x0, dt );
	SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
	SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt );
}

void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt )
{
	add_source ( N, u, u0, dt ); add_source ( N, v, v0, dt );
	SWAP ( u0, u ); diffuse ( N, 1, u, u0, visc, dt );
	SWAP ( v0, v ); diffuse ( N, 2, v, v0, visc, dt );
	project ( N, u, v, u0, v0 );
	SWAP ( u0, u ); SWAP ( v0, v );
	advect ( N, 1, u, u0, u0, v0, dt ); advect ( N, 2, v, v0, u0, v0, dt );
	project ( N, u, v, u0, v0 );
}

void wave_step ( int N, float * buf, float * buf1, float * buf2, float *dampening, float timePass, float *velocity[2] )
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
			u[x] = 10 * dump[x] *  (row1[x-1] + row1[x+1] - (2 * row[x])) * dt;
			v[x] = 10 * dump[x] *  (row1up[x]+row1down[x] - (2 * row[x])) * dt;
			float force = u[x] + v[x];
			float newHight =  (1.99 * row1[x]) - (0.99 * row2[x]) + 0.1 * force;
			row[x] = newHight;
		}
	}
}


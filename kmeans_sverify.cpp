#include <stdio.h>
#include <iostream>
#include <time.h>

#include <ac_int.h>
#include <ac_math.h>
#include <mc_scverify.h>

struct Point {
	ac_int<16, false> x;
	ac_int<16, false> y;
};

inline ac_int<17, false> CCS_BLOCK(Manhatan)(Point& center, ac_int<16, false> &x, ac_int<16, false> &y) {
	ac_int<16, false> abs_x, abs_y;

	ac_math::ac_abs(center.x - x, abs_x);
	ac_math::ac_abs(center.y - y, abs_y);
	
	return abs_x + abs_y;
}
#pragma hls_design top
template<int N, int M>
bool CCS_BLOCK(kmeans)(Point points[N], int ID[N], Point center[M]) {

	ac_int<16, false> temp_x, temp_y, dist_best, dist_new, sum_all_centers_new , sum_all_centers_old ;
	ac_int<16, false> change = 0, K[M] = { 0,0,0 };
	//ac_fixed<4,2, false> change;//max 4 min 0.25
	Point sum_new_centerst[M] = {};
	bool flage;
	int temp_id ;

	POINTS: for (int i = 0; i < N; i++) {
		// read point ({example} ac_int<16,false> x = points[i].x; )
		temp_x = points[i].x;
		temp_y = points[i].y;

		temp_id = 0 ;
		dist_best = -1;//set dist to maximum value of a unsigned
		DIST: for (int j = 0; j < M; j++) {
			// calculate distance to each center with Mahalanobis dist

			dist_new = Manhatan(center[j],temp_x,temp_y);

			// find minimum distance (Max temp_id = M)
			if (dist_new < dist_best) {
				temp_id =  j ;//change id
				dist_best = dist_new;//best distance
			}

		}
		// update the ID[i] with the id of the closest center
		ID[i] = temp_id;

	// update sum for new centers			
	sum_new_centerst[temp_id].x += temp_x;	
	sum_new_centerst[temp_id].y += temp_y;

	//number of nodes in class with id temp_id
	K[temp_id]++;

	}

	// update centers
	UPDATE: for (int j = 0; j < M; j++) {
	// find old centers sum
	sum_all_centers_old = center[j].x + center[j].y;

	ac_math::ac_div(sum_new_centerst[j].x, K[j], temp_x);
	ac_math::ac_div(sum_new_centerst[j].y, K[j], temp_y);

	// save new centers change
	sum_all_centers_new = temp_y + temp_x;

	center[j].x = temp_x;
	center[j].y = temp_y;

	}

	// if new and old centers are almost similar return false, else return true
	//change = abs(sum_all_centers_old - sum_all_centers_new);
	ac_math::ac_abs(sum_all_centers_old - sum_all_centers_new , change);
	flage = (change < 1) ? false : true;

	return flage;
}

template<int N, int M ,int max>
void initialize(Point points[N], Point centers[M], int ID[N]) {
	srand(123);

	for (int i = 0; i < N; i++) {
		points[i].x = rand() % max ; 
		points[i].y = rand() % max ;

		ID[i] = 0;// (N % M);
		std::cout << i << " " << ID[i] << std::endl;
	}

	for (int i = 0; i < M; i++) {
		centers[i].x = rand() % max;
		centers[i].y = rand() % max;
	}


}

CCS_MAIN(int argc, char* argv[]) {
	//test function i need//
	ac_int<16, true> a = 1 ;
	ac_int<16, true> b = 2;
	//--------------------//
	ac_math::ac_abs(a - b, a);
	ac_math::ac_div(a, b, a);

	const int N = 50;
	const int M = 3;
	const int max = 50;

	Point points[N], center[M];
	int ID[N]; 

	initialize<N, M , max>(points, center, ID);

	bool go_on = true;
	int repetitions = 0;
	while (go_on) {
		go_on = kmeans<N, M>(points, ID, center);
		repetitions++;
	}

	std::cout << "The 3 centers are:" << std::endl;
	std::cout << center[0].x << ", " << center[0].y << std::endl;
	std::cout << center[1].x << ", " << center[1].y << std::endl;
	std::cout << center[2].x << ", " << center[2].y << std::endl;
	std::cout <<  "after total repetitions :" << repetitions << std::endl;

	for (int i = 0; i < N; i++) {
		std::cout << points[i].x << "," << points[i].y << " :" << ID[i] << std::endl;
	}

	CCS_RETURN(0);
}

#pragma once

#include <iostream>
#include <assimp/scene.h>
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
using namespace std;

///// Credit for following 3 functions: Dr. Michael J. Reale /////
void aiMatToGLM4(aiMatrix4x4 &a, glm::mat4 &m);
void printTab(int cnt);
void printNodeInfo(aiNode *node, glm::mat4 &nodeT, glm::mat4 &parentMat, glm::mat4 &currentMat, int level);

	/// Sign stuff ///

template<typename T>
inline bool isNegative(T x) {
	return (x < 0);
};

// Return the sign
template<typename T>
inline T sign(T x) {
	if (isNegative(x))
		return static_cast<T>(-1);
	else
		return static_cast<T>(1);
};
// Return the negation of the sign
template<typename T>
inline T negSign(T x) {
	if (isNegative(x))
		return static_cast<T>(1);
	else
		return static_cast<T>(-1);
};

// Flips the sign of value if flip is negative
template <typename T, typename U>
inline T flipBy(T value, U flip) {
	if (isNegative(flip))
		return -value;
	else
		return value;
};

// Returns value with its sign matching the sign of flip
template <typename T, typename U>
inline T setSign(T value, U set) {
	if (isNegative(value) == isNegative(set))
		return value;
	else
		return -value;
};

	/// Comparisons with vectors ///

// Given two vectors, return the max value for each axis
template <size_t N, typename T, glm::qualifier Q>
inline glm::vec<N,T,Q> maxAxes(glm::vec<N,T,Q> &v1, glm::vec<N,T,Q> &v2) {
	glm::vec<N,T,Q> nv;
	for (int i = 0; i < N; i++) {
		nv[i] = std::max(v1[i], v2[i]);
	}
	return nv;
};
// Given two vectors, return the min value for each axis
template <size_t N, typename T, glm::qualifier Q>
inline glm::vec<N,T,Q> minAxes(glm::vec<N,T,Q> &v1, glm::vec<N,T,Q> &v2) {
	glm::vec<N,T,Q> nv;
	for (int i = 0; i < N; i++) {
		nv[i] = std::min(v1[i], v2[i]);
	}
	return nv;
};

// Modifies a vector to bound its values to between the min and max. Swaps min/max if min > max.
template <size_t N, typename T, glm::qualifier Q>
inline void boundBetween(glm::vec<N,T,Q> &v, float min, float max) {
	if (min > max) { std::swap(min, max); }

	for (size_t i = 0; i < N; i++) {
		if (v[i] < min)
			v[i] = min;
		else if (v[i] > max)
			v[i] = max;
	}
};


	/// Printing/debugging ///

template <size_t N, typename T, glm::qualifier Q>
inline void print(glm::vec<N,T,Q> &v) {
	cout << "{" << v[0];
	for (int i = 1; i < N; i++) {
		cout << ", " << v[i];
	}
	cout << "}" << endl;
}

template <size_t N, typename T, size_t M, glm::qualifier Q>
inline void print(glm::mat<N,M,T,Q> &mat) {
	cout << "[" << endl;
	for (size_t n = 0; n < N; n++) {
		cout << "\t{" << mat[0][n];
		for (int m = 1; m < M; m++) {
			cout << ", " << mat[m][n];
		}
		cout << "}" << endl;
	}
	cout << "]" << endl;
}


	/// Matrix functions ///

// Builds a scalar matrix. Must manually include homogenous coordniates.
template <size_t N, typename T, glm::qualifier Q>
inline glm::mat<N,N,T,Q> buildScale(glm::vec<N,T,Q> &scalar) {
	glm::mat<N,N,T,Q> mat = glm::mat<N,N,T,Q>();
	
	for (size_t i = 0; i < N; i++) {
		mat[i][i] = scalar[i];
	}

	return mat;
}

// Builds a translation matrix. Must manually include homogenous coordniates.
template <size_t N, typename T, glm::qualifier Q>
inline glm::mat<N,N,T,Q> buildTranslate(glm::vec<N,T,Q> &offset) {
	glm::mat<N,N,T,Q> mat = glm::mat<N,N,T,Q>(1.0);
	
	for (size_t i = 0; i < N; i++) {
		mat[N-1][i] = offset[i];
	}

	return mat;
}

// Increases the matrix size by the specified amount (places 1s on diagonal). Usage: bigMat = increaseMatrixSize<inc_cnt>(smallMat)
template <size_t I, size_t N, typename T, glm::qualifier Q>
inline glm::mat<N+I,N+I,T,Q> increaseMatrixSize(glm::mat<N,N,T,Q> &oldMat) {
	glm::mat<N+I,N+I,T,Q> newMat = glm::mat<N+I,N+I,T,Q>();

	for (size_t i = 0; i < N; i++) {
		// Copy existing values over from oldMat
		for (size_t j = 0; j < N; j++) {
			newMat[i][j] = oldMat[i][j];
		}
		
		// For remaingin empty spaces, fill diagonal with 1s and rest with 0s
		for (size_t k = N; k < N+I; k++) {
			newMat[i][k] = 0.0;
			newMat[k][i] = 0.0;
			newMat[k][k] = 1.0;
		}
	}

	return newMat;
}

// Decreases the matrix size by the specified amount (removes from bottom/right). Usage: smallMat = decreaseMatrixSize<inc_cnt>(bigMat)
template <size_t D, size_t N, typename T, glm::qualifier Q>
inline glm::mat<N-D,N-D,T,Q> decreaseMatrixSize(glm::mat<N,N,T,Q> &oldMat) {
	glm::mat<N-D,N-D,T,Q> newMat = glm::mat<N-D,N-D,T,Q>();

	for (size_t i = 0; i < N-D; i++) {
		for (size_t j = 0; j < N-D; j++) {
			newMat[i][j] = oldMat[i][j];
		}
	}

	return newMat;
}
#pragma once
#include <cstdint>
#include <cstddef>
#include <ostream>

// ---------------------------------------------------------------- Vectors
template <typename T>
struct Vec4 {
private:
    T v[4];
public:
    constexpr Vec4() : v{} {}
    template <typename U>
    constexpr Vec4(const Vec4<U> _v) : v{} {v[0] = static_cast<T>(_v(0)); v[1] = static_cast<T>(_v(1)); v[2] = static_cast<T>(_v(2)); v[3] = static_cast<T>(_v(3));}
    constexpr Vec4(const T (&values)[4]) : v{values[0], values[1], values[2], values[3]} {}
    constexpr T& operator()(const uint8_t i) { return v[i]; }
    constexpr const T& operator()(const uint8_t i) const { return v[i]; }
};

template <typename T>
struct Vec3 {
private:
    T v[3];
public:
    constexpr Vec3() : v{} {}
    template <typename U>
    constexpr Vec3(const Vec3<U> _v) : v{} {v[0] = static_cast<T>(_v(0)); v[1] = static_cast<T>(_v(1)); v[2] = static_cast<T>(_v(2));}
    constexpr Vec3(const T (&values)[3]) : v{values[0], values[1], values[2]} {}
    constexpr T& operator()(const uint8_t i) { return v[i]; }
    constexpr const T& operator()(const uint8_t i) const { return v[i]; }
};

template <typename T>
struct Vec2 {
private:
    T v[2];
public:
    constexpr Vec2() : v{} {}
    template <typename U>
    constexpr Vec2(const Vec2<U> _v) : v{} {v[0] = static_cast<T>(_v(0)); v[1] = static_cast<T>(_v(1));}
    constexpr Vec2(const T (&values)[2]) : v{values[0], values[1]} {}
    constexpr T& operator()(const uint8_t i) { return v[i]; }
    constexpr const T& operator()(const uint8_t i) const { return v[i]; }

    constexpr T& x(){return v[0];}
    constexpr const T& x() const{return v[0];}
    constexpr T& y(){return v[1];}
    constexpr const T& y() const{return v[1];}
};

// ---------------------------------------------------------------- Matrices
// Access M(r,c) and brace-init are row-major (reads like the matrix on paper).
// Storage is column-major: M(r,c) = m[c * rows + r], so each column is contiguous.
// The single-index operator()(i) indexes raw storage order, not row-major order.
template <typename T>
struct Mat4 {
private:
    T m[16];
public:
    constexpr Mat4() : m{} {}
    // Input is row-major (reads like the matrix on paper); storage is column-major.
    constexpr Mat4(const T (&values)[16]) : m{} {
        m[0] = values[0]; m[1] = values[4]; m[2] = values[8]; m[3] = values[12];   // column 0
        m[4] = values[1]; m[5] = values[5]; m[6] = values[9]; m[7] = values[13];   // column 1
        m[8] = values[2]; m[9] = values[6]; m[10] = values[10]; m[11] = values[14];   // column 2
        m[12] = values[3]; m[13] = values[7]; m[14] = values[11]; m[15] = values[15];   // column 3
    }
    constexpr T& operator()(const uint8_t r, const uint8_t c) { return m[c * 4 + r]; }
    constexpr const T& operator()(const uint8_t r, const uint8_t c) const { return m[c * 4 + r]; }
    constexpr T& operator()(const uint8_t i) { return m[i]; }          // raw storage order (column-major)
    constexpr const T& operator()(const uint8_t i) const { return m[i]; }
};

template <typename T>
struct Mat3 {
private:
    T m[9];
public:
    constexpr Mat3() : m{} {}
    // Input is row-major (reads like the matrix on paper); storage is column-major.
    constexpr Mat3(const T (&values)[9]) : m{} {
        m[0] = values[0]; m[1] = values[3]; m[2] = values[6];   // column 0
        m[3] = values[1]; m[4] = values[4]; m[5] = values[7];   // column 1
        m[6] = values[2]; m[7] = values[5]; m[8] = values[8];   // column 2
    }
    constexpr T& operator()(const uint8_t r, const uint8_t c) { return m[c * 3 + r]; }
    constexpr const T& operator()(const uint8_t r, const uint8_t c) const { return m[c * 3 + r]; }
    constexpr T& operator()(const uint8_t i) { return m[i]; }          // raw storage order (column-major)
    constexpr const T& operator()(const uint8_t i) const { return m[i]; }
};

template <typename T>
struct Mat2 {
private:
    T m[4];
public:
    constexpr Mat2() : m{} {}
    // Input is row-major (reads like the matrix on paper); storage is column-major.
    constexpr Mat2(const T (&values)[4]) : m{} {
        m[0] = values[0]; m[1] = values[2];   // column 0
        m[2] = values[1]; m[3] = values[3];   // column 1
    }
    constexpr T& operator()(const uint8_t r, const uint8_t c) { return m[c * 2 + r]; }
    constexpr const T& operator()(const uint8_t r, const uint8_t c) const { return m[c * 2 + r]; }
    constexpr T& operator()(const uint8_t i) { return m[i]; }          // raw storage order (column-major)
    constexpr const T& operator()(const uint8_t i) const { return m[i]; }
};

// 2 rows, 3 columns
template <typename T>
struct Mat2x3 {
private:
    T m[6];
public:
    constexpr Mat2x3() : m{} {}
    // Input is row-major (reads like the matrix on paper); storage is column-major.
    constexpr Mat2x3(const T (&values)[6]) : m{} {
        m[0] = values[0]; m[1] = values[3];   // column 0
        m[2] = values[1]; m[3] = values[4];   // column 1
        m[4] = values[2]; m[5] = values[5];   // column 2
    }
    constexpr T& operator()(const uint8_t r, const uint8_t c) { return m[c * 2 + r]; }
    constexpr const T& operator()(const uint8_t r, const uint8_t c) const { return m[c * 2 + r]; }
    constexpr T& operator()(const uint8_t i) { return m[i]; }          // raw storage order (column-major)
    constexpr const T& operator()(const uint8_t i) const { return m[i]; }
};

// 3 rows, 2 columns
template <typename T>
struct Mat3x2 {
private:
    T m[6];
public:
    constexpr Mat3x2() : m{} {}
    // Input is row-major (reads like the matrix on paper); storage is column-major.
    constexpr Mat3x2(const T (&values)[6]) : m{} {
        m[0] = values[0]; m[1] = values[2]; m[2] = values[4];   // column 0
        m[3] = values[1]; m[4] = values[3]; m[5] = values[5];   // column 1
    }
    constexpr T& operator()(const uint8_t r, const uint8_t c) { return m[c * 3 + r]; }
    constexpr const T& operator()(const uint8_t r, const uint8_t c) const { return m[c * 3 + r]; }
    constexpr T& operator()(const uint8_t i) { return m[i]; }          // raw storage order (column-major)
    constexpr const T& operator()(const uint8_t i) const { return m[i]; }
};

// 2 rows, 4 columns
template <typename T>
struct Mat2x4 {
private:
    T m[8];
public:
    constexpr Mat2x4() : m{} {}
    // Input is row-major (reads like the matrix on paper); storage is column-major.
    constexpr Mat2x4(const T (&values)[8]) : m{} {
        m[0] = values[0]; m[1] = values[4];   // column 0
        m[2] = values[1]; m[3] = values[5];   // column 1
        m[4] = values[2]; m[5] = values[6];   // column 2
        m[6] = values[3]; m[7] = values[7];   // column 3
    }
    constexpr T& operator()(const uint8_t r, const uint8_t c) { return m[c * 2 + r]; }
    constexpr const T& operator()(const uint8_t r, const uint8_t c) const { return m[c * 2 + r]; }
    constexpr T& operator()(const uint8_t i) { return m[i]; }          // raw storage order (column-major)
    constexpr const T& operator()(const uint8_t i) const { return m[i]; }
};

// 4 rows, 2 columns
template <typename T>
struct Mat4x2 {
private:
    T m[8];
public:
    constexpr Mat4x2() : m{} {}
    // Input is row-major (reads like the matrix on paper); storage is column-major.
    constexpr Mat4x2(const T (&values)[8]) : m{} {
        m[0] = values[0]; m[1] = values[2]; m[2] = values[4]; m[3] = values[6];   // column 0
        m[4] = values[1]; m[5] = values[3]; m[6] = values[5]; m[7] = values[7];   // column 1
    }
    constexpr T& operator()(const uint8_t r, const uint8_t c) { return m[c * 4 + r]; }
    constexpr const T& operator()(const uint8_t r, const uint8_t c) const { return m[c * 4 + r]; }
    constexpr T& operator()(const uint8_t i) { return m[i]; }          // raw storage order (column-major)
    constexpr const T& operator()(const uint8_t i) const { return m[i]; }
};

// 3 rows, 4 columns
template <typename T>
struct Mat3x4 {
private:
    T m[12];
public:
    constexpr Mat3x4() : m{} {}
    // Input is row-major (reads like the matrix on paper); storage is column-major.
    constexpr Mat3x4(const T (&values)[12]) : m{} {
        m[0] = values[0]; m[1] = values[4]; m[2] = values[8];   // column 0
        m[3] = values[1]; m[4] = values[5]; m[5] = values[9];   // column 1
        m[6] = values[2]; m[7] = values[6]; m[8] = values[10];   // column 2
        m[9] = values[3]; m[10] = values[7]; m[11] = values[11];   // column 3
    }
    constexpr T& operator()(const uint8_t r, const uint8_t c) { return m[c * 3 + r]; }
    constexpr const T& operator()(const uint8_t r, const uint8_t c) const { return m[c * 3 + r]; }
    constexpr T& operator()(const uint8_t i) { return m[i]; }          // raw storage order (column-major)
    constexpr const T& operator()(const uint8_t i) const { return m[i]; }
};

// 4 rows, 3 columns
template <typename T>
struct Mat4x3 {
private:
    T m[12];
public:
    constexpr Mat4x3() : m{} {}
    // Input is row-major (reads like the matrix on paper); storage is column-major.
    constexpr Mat4x3(const T (&values)[12]) : m{} {
        m[0] = values[0]; m[1] = values[3]; m[2] = values[6]; m[3] = values[9];   // column 0
        m[4] = values[1]; m[5] = values[4]; m[6] = values[7]; m[7] = values[10];   // column 1
        m[8] = values[2]; m[9] = values[5]; m[10] = values[8]; m[11] = values[11];   // column 2
    }
    constexpr T& operator()(const uint8_t r, const uint8_t c) { return m[c * 4 + r]; }
    constexpr const T& operator()(const uint8_t r, const uint8_t c) const { return m[c * 4 + r]; }
    constexpr T& operator()(const uint8_t i) { return m[i]; }          // raw storage order (column-major)
    constexpr const T& operator()(const uint8_t i) const { return m[i]; }
};

// ---------------------------------------------------------------- Aliases
using Vec2f = Vec2<float>;     using Vec2d = Vec2<double>;
using Vec3f = Vec3<float>;     using Vec3d = Vec3<double>;
using Vec4f = Vec4<float>;     using Vec4d = Vec4<double>;
using Mat2f = Mat2<float>;     using Mat2d = Mat2<double>;
using Mat3f = Mat3<float>;     using Mat3d = Mat3<double>;
using Mat4f = Mat4<float>;     using Mat4d = Mat4<double>;
using Mat2x3f = Mat2x3<float>; using Mat2x3d = Mat2x3<double>;
using Mat3x2f = Mat3x2<float>; using Mat3x2d = Mat3x2<double>;
using Mat2x4f = Mat2x4<float>; using Mat2x4d = Mat2x4<double>;
using Mat4x2f = Mat4x2<float>; using Mat4x2d = Mat4x2<double>;
using Mat3x4f = Mat3x4<float>; using Mat3x4d = Mat3x4<double>;
using Mat4x3f = Mat4x3<float>; using Mat4x3d = Mat4x3<double>;

// ---------------------------------------------------------------- Matrix * Vector
template <typename T, typename U>
constexpr Vec2<T> operator*(const Mat2<T>& m, const Vec2<U>& v) {
    Vec2<T> o;
    o(0) = m(0,0)*v(0) + m(0,1)*v(1);
    o(1) = m(1,0)*v(0) + m(1,1)*v(1);   // fixed: original used v(1) in the first term
    return o;
}

template <typename T, typename U>
constexpr Vec3<T> operator*(const Mat3<T>& m, const Vec3<U>& v) {
    Vec3<T> o;
    o(0) = m(0,0)*v(0) + m(0,1)*v(1) + m(0,2)*v(2);
    o(1) = m(1,0)*v(0) + m(1,1)*v(1) + m(1,2)*v(2);
    o(2) = m(2,0)*v(0) + m(2,1)*v(1) + m(2,2)*v(2);
    return o;
}

template <typename T, typename U>
constexpr Vec4<T> operator*(const Mat4<T>& m, const Vec4<U>& v) {
    Vec4<T> o;
    o(0) = m(0,0)*v(0) + m(0,1)*v(1) + m(0,2)*v(2) + m(0,3)*v(3);
    o(1) = m(1,0)*v(0) + m(1,1)*v(1) + m(1,2)*v(2) + m(1,3)*v(3);
    o(2) = m(2,0)*v(0) + m(2,1)*v(1) + m(2,2)*v(2) + m(2,3)*v(3);
    o(3) = m(3,0)*v(0) + m(3,1)*v(1) + m(3,2)*v(2) + m(3,3)*v(3);
    return o;
}

template <typename T, typename U>
constexpr Vec2<T> operator*(const Mat2x3<T>& m, const Vec3<U>& v) {
    Vec2<T> o;
    o(0) = m(0,0)*v(0) + m(0,1)*v(1) + m(0,2)*v(2);
    o(1) = m(1,0)*v(0) + m(1,1)*v(1) + m(1,2)*v(2);
    return o;
}

template <typename T, typename U>
constexpr Vec3<T> operator*(const Mat3x2<T>& m, const Vec2<U>& v) {
    Vec3<T> o;
    o(0) = m(0,0)*v(0) + m(0,1)*v(1);
    o(1) = m(1,0)*v(0) + m(1,1)*v(1);
    o(2) = m(2,0)*v(0) + m(2,1)*v(1);
    return o;
}

template <typename T, typename U>
constexpr Vec2<T> operator*(const Mat2x4<T>& m, const Vec4<U>& v) {
    Vec2<T> o;
    o(0) = m(0,0)*v(0) + m(0,1)*v(1) + m(0,2)*v(2) + m(0,3)*v(3);
    o(1) = m(1,0)*v(0) + m(1,1)*v(1) + m(1,2)*v(2) + m(1,3)*v(3);
    return o;
}

template <typename T, typename U>
constexpr Vec4<T> operator*(const Mat4x2<T>& m, const Vec2<U>& v) {
    Vec4<T> o;
    o(0) = m(0,0)*v(0) + m(0,1)*v(1);
    o(1) = m(1,0)*v(0) + m(1,1)*v(1);
    o(2) = m(2,0)*v(0) + m(2,1)*v(1);
    o(3) = m(3,0)*v(0) + m(3,1)*v(1);
    return o;
}

template <typename T, typename U>
constexpr Vec3<T> operator*(const Mat3x4<T>& m, const Vec4<U>& v) {
    Vec3<T> o;
    o(0) = m(0,0)*v(0) + m(0,1)*v(1) + m(0,2)*v(2) + m(0,3)*v(3);
    o(1) = m(1,0)*v(0) + m(1,1)*v(1) + m(1,2)*v(2) + m(1,3)*v(3);
    o(2) = m(2,0)*v(0) + m(2,1)*v(1) + m(2,2)*v(2) + m(2,3)*v(3);
    return o;
}

template <typename T, typename U>
constexpr Vec4<T> operator*(const Mat4x3<T>& m, const Vec3<U>& v) {
    Vec4<T> o;
    o(0) = m(0,0)*v(0) + m(0,1)*v(1) + m(0,2)*v(2);
    o(1) = m(1,0)*v(0) + m(1,1)*v(1) + m(1,2)*v(2);
    o(2) = m(2,0)*v(0) + m(2,1)*v(1) + m(2,2)*v(2);
    o(3) = m(3,0)*v(0) + m(3,1)*v(1) + m(3,2)*v(2);
    return o;
}

// ---------------------------------------------------------------- Vector * Scalar
template <typename T, typename U>
constexpr Vec2<T> operator*(const Vec2<T> &v, const U scalar){
    return {v(0) * scalar, v(1) * scalar};
}

template <typename T, typename U>
constexpr Vec2<T> operator*(const U scalar, const Vec2<T> &v){
    return {v(0) * scalar, v(1) * scalar};
}

template <typename T, typename U>
constexpr Vec3<T> operator*(const Vec3<T> &v, const U scalar){
    return {v(0) * scalar, v(1) * scalar, v(2) * scalar};
}

template <typename T, typename U>
constexpr Vec3<T> operator*(const U scalar, const Vec3<T> &v){
    return {v(0) * scalar, v(1) * scalar, v(2) * scalar};
}

template <typename T, typename U>
constexpr Vec4<T> operator*(const Vec4<T> &v, const U scalar){
    return {v(0) * scalar, v(1) * scalar, v(2) * scalar, v(3) * scalar};
}

template <typename T, typename U>
constexpr Vec4<T> operator*(const U scalar, const Vec4<T> &v){
    return {v(0) * scalar, v(1) * scalar, v(2) * scalar, v(3) * scalar};
}

// ---------------------------------------------------------------- Matrix * Matrix
// Naming: A(r x k) * B(k x c) -> (r x c)

// ---- 2 x k
template <typename T, typename U>
constexpr Mat2<T> operator*(const Mat2<T>& m1, const Mat2<U>& m2) {
    Mat2<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1);

    return o;
}

template <typename T, typename U>
constexpr Mat2x3<T> operator*(const Mat2<T>& m1, const Mat2x3<U>& m2) {
    Mat2x3<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2);

    return o;
}

template <typename T, typename U>
constexpr Mat2x4<T> operator*(const Mat2<T>& m1, const Mat2x4<U>& m2) {
    Mat2x4<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2);
    o(0,3) = m1(0,0)*m2(0,3) + m1(0,1)*m2(1,3);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2);
    o(1,3) = m1(1,0)*m2(0,3) + m1(1,1)*m2(1,3);

    return o;
}

template <typename T, typename U>
constexpr Mat2<T> operator*(const Mat2x3<T>& m1, const Mat3x2<U>& m2) {
    Mat2<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1);

    return o;
}

template <typename T, typename U>
constexpr Mat2x3<T> operator*(const Mat2x3<T>& m1, const Mat3<U>& m2) {
    Mat2x3<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2);

    return o;
}

template <typename T, typename U>
constexpr Mat2x4<T> operator*(const Mat2x3<T>& m1, const Mat3x4<U>& m2) {
    Mat2x4<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2);
    o(0,3) = m1(0,0)*m2(0,3) + m1(0,1)*m2(1,3) + m1(0,2)*m2(2,3);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2);
    o(1,3) = m1(1,0)*m2(0,3) + m1(1,1)*m2(1,3) + m1(1,2)*m2(2,3);

    return o;
}

template <typename T, typename U>
constexpr Mat2<T> operator*(const Mat2x4<T>& m1, const Mat4x2<U>& m2) {
    Mat2<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0) + m1(0,3)*m2(3,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1) + m1(0,3)*m2(3,1);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0) + m1(1,3)*m2(3,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1) + m1(1,3)*m2(3,1);

    return o;
}

template <typename T, typename U>
constexpr Mat2x3<T> operator*(const Mat2x4<T>& m1, const Mat4x3<U>& m2) {
    Mat2x3<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0) + m1(0,3)*m2(3,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1) + m1(0,3)*m2(3,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2) + m1(0,3)*m2(3,2);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0) + m1(1,3)*m2(3,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1) + m1(1,3)*m2(3,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2) + m1(1,3)*m2(3,2);

    return o;
}

template <typename T, typename U>
constexpr Mat2x4<T> operator*(const Mat2x4<T>& m1, const Mat4<U>& m2) {
    Mat2x4<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0) + m1(0,3)*m2(3,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1) + m1(0,3)*m2(3,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2) + m1(0,3)*m2(3,2);
    o(0,3) = m1(0,0)*m2(0,3) + m1(0,1)*m2(1,3) + m1(0,2)*m2(2,3) + m1(0,3)*m2(3,3);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0) + m1(1,3)*m2(3,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1) + m1(1,3)*m2(3,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2) + m1(1,3)*m2(3,2);
    o(1,3) = m1(1,0)*m2(0,3) + m1(1,1)*m2(1,3) + m1(1,2)*m2(2,3) + m1(1,3)*m2(3,3);

    return o;
}

// ---- 3 x k
template <typename T, typename U>
constexpr Mat3x2<T> operator*(const Mat3x2<T>& m1, const Mat2<U>& m2) {
    Mat3x2<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1);

    return o;
}

template <typename T, typename U>
constexpr Mat3<T> operator*(const Mat3x2<T>& m1, const Mat2x3<U>& m2) {
    Mat3<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2);

    return o;
}

template <typename T, typename U>
constexpr Mat3x4<T> operator*(const Mat3x2<T>& m1, const Mat2x4<U>& m2) {
    Mat3x4<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2);
    o(0,3) = m1(0,0)*m2(0,3) + m1(0,1)*m2(1,3);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2);
    o(1,3) = m1(1,0)*m2(0,3) + m1(1,1)*m2(1,3);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2);
    o(2,3) = m1(2,0)*m2(0,3) + m1(2,1)*m2(1,3);

    return o;
}

template <typename T, typename U>
constexpr Mat3x2<T> operator*(const Mat3<T>& m1, const Mat3x2<U>& m2) {
    Mat3x2<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1);

    return o;
}

template <typename T, typename U>
constexpr Mat3<T> operator*(const Mat3<T>& m1, const Mat3<U>& m2) {
    Mat3<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2) + m1(2,2)*m2(2,2);

    return o;
}

template <typename T, typename U>
constexpr Mat3x4<T> operator*(const Mat3<T>& m1, const Mat3x4<U>& m2) {
    Mat3x4<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2);
    o(0,3) = m1(0,0)*m2(0,3) + m1(0,1)*m2(1,3) + m1(0,2)*m2(2,3);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2);
    o(1,3) = m1(1,0)*m2(0,3) + m1(1,1)*m2(1,3) + m1(1,2)*m2(2,3);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2) + m1(2,2)*m2(2,2);
    o(2,3) = m1(2,0)*m2(0,3) + m1(2,1)*m2(1,3) + m1(2,2)*m2(2,3);

    return o;
}

template <typename T, typename U>
constexpr Mat3x2<T> operator*(const Mat3x4<T>& m1, const Mat4x2<U>& m2) {
    Mat3x2<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0) + m1(0,3)*m2(3,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1) + m1(0,3)*m2(3,1);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0) + m1(1,3)*m2(3,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1) + m1(1,3)*m2(3,1);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0) + m1(2,3)*m2(3,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1) + m1(2,3)*m2(3,1);

    return o;
}

template <typename T, typename U>
constexpr Mat3<T> operator*(const Mat3x4<T>& m1, const Mat4x3<U>& m2) {
    Mat3<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0) + m1(0,3)*m2(3,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1) + m1(0,3)*m2(3,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2) + m1(0,3)*m2(3,2);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0) + m1(1,3)*m2(3,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1) + m1(1,3)*m2(3,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2) + m1(1,3)*m2(3,2);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0) + m1(2,3)*m2(3,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1) + m1(2,3)*m2(3,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2) + m1(2,2)*m2(2,2) + m1(2,3)*m2(3,2);

    return o;
}

template <typename T, typename U>
constexpr Mat3x4<T> operator*(const Mat3x4<T>& m1, const Mat4<U>& m2) {
    Mat3x4<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0) + m1(0,3)*m2(3,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1) + m1(0,3)*m2(3,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2) + m1(0,3)*m2(3,2);
    o(0,3) = m1(0,0)*m2(0,3) + m1(0,1)*m2(1,3) + m1(0,2)*m2(2,3) + m1(0,3)*m2(3,3);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0) + m1(1,3)*m2(3,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1) + m1(1,3)*m2(3,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2) + m1(1,3)*m2(3,2);
    o(1,3) = m1(1,0)*m2(0,3) + m1(1,1)*m2(1,3) + m1(1,2)*m2(2,3) + m1(1,3)*m2(3,3);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0) + m1(2,3)*m2(3,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1) + m1(2,3)*m2(3,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2) + m1(2,2)*m2(2,2) + m1(2,3)*m2(3,2);
    o(2,3) = m1(2,0)*m2(0,3) + m1(2,1)*m2(1,3) + m1(2,2)*m2(2,3) + m1(2,3)*m2(3,3);

    return o;
}

// ---- 4 x k
template <typename T, typename U>
constexpr Mat4x2<T> operator*(const Mat4x2<T>& m1, const Mat2<U>& m2) {
    Mat4x2<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1);

    o(3,0) = m1(3,0)*m2(0,0) + m1(3,1)*m2(1,0);
    o(3,1) = m1(3,0)*m2(0,1) + m1(3,1)*m2(1,1);

    return o;
}

template <typename T, typename U>
constexpr Mat4x3<T> operator*(const Mat4x2<T>& m1, const Mat2x3<U>& m2) {
    Mat4x3<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2);

    o(3,0) = m1(3,0)*m2(0,0) + m1(3,1)*m2(1,0);
    o(3,1) = m1(3,0)*m2(0,1) + m1(3,1)*m2(1,1);
    o(3,2) = m1(3,0)*m2(0,2) + m1(3,1)*m2(1,2);

    return o;
}

template <typename T, typename U>
constexpr Mat4<T> operator*(const Mat4x2<T>& m1, const Mat2x4<U>& m2) {
    Mat4<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2);
    o(0,3) = m1(0,0)*m2(0,3) + m1(0,1)*m2(1,3);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2);
    o(1,3) = m1(1,0)*m2(0,3) + m1(1,1)*m2(1,3);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2);
    o(2,3) = m1(2,0)*m2(0,3) + m1(2,1)*m2(1,3);

    o(3,0) = m1(3,0)*m2(0,0) + m1(3,1)*m2(1,0);
    o(3,1) = m1(3,0)*m2(0,1) + m1(3,1)*m2(1,1);
    o(3,2) = m1(3,0)*m2(0,2) + m1(3,1)*m2(1,2);
    o(3,3) = m1(3,0)*m2(0,3) + m1(3,1)*m2(1,3);

    return o;
}

template <typename T, typename U>
constexpr Mat4x2<T> operator*(const Mat4x3<T>& m1, const Mat3x2<U>& m2) {
    Mat4x2<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1);

    o(3,0) = m1(3,0)*m2(0,0) + m1(3,1)*m2(1,0) + m1(3,2)*m2(2,0);
    o(3,1) = m1(3,0)*m2(0,1) + m1(3,1)*m2(1,1) + m1(3,2)*m2(2,1);

    return o;
}

template <typename T, typename U>
constexpr Mat4x3<T> operator*(const Mat4x3<T>& m1, const Mat3<U>& m2) {
    Mat4x3<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2) + m1(2,2)*m2(2,2);

    o(3,0) = m1(3,0)*m2(0,0) + m1(3,1)*m2(1,0) + m1(3,2)*m2(2,0);
    o(3,1) = m1(3,0)*m2(0,1) + m1(3,1)*m2(1,1) + m1(3,2)*m2(2,1);
    o(3,2) = m1(3,0)*m2(0,2) + m1(3,1)*m2(1,2) + m1(3,2)*m2(2,2);

    return o;
}

template <typename T, typename U>
constexpr Mat4<T> operator*(const Mat4x3<T>& m1, const Mat3x4<U>& m2) {
    Mat4<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2);
    o(0,3) = m1(0,0)*m2(0,3) + m1(0,1)*m2(1,3) + m1(0,2)*m2(2,3);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2);
    o(1,3) = m1(1,0)*m2(0,3) + m1(1,1)*m2(1,3) + m1(1,2)*m2(2,3);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2) + m1(2,2)*m2(2,2);
    o(2,3) = m1(2,0)*m2(0,3) + m1(2,1)*m2(1,3) + m1(2,2)*m2(2,3);

    o(3,0) = m1(3,0)*m2(0,0) + m1(3,1)*m2(1,0) + m1(3,2)*m2(2,0);
    o(3,1) = m1(3,0)*m2(0,1) + m1(3,1)*m2(1,1) + m1(3,2)*m2(2,1);
    o(3,2) = m1(3,0)*m2(0,2) + m1(3,1)*m2(1,2) + m1(3,2)*m2(2,2);
    o(3,3) = m1(3,0)*m2(0,3) + m1(3,1)*m2(1,3) + m1(3,2)*m2(2,3);

    return o;
}

template <typename T, typename U>
constexpr Mat4x2<T> operator*(const Mat4<T>& m1, const Mat4x2<U>& m2) {
    Mat4x2<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0) + m1(0,3)*m2(3,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1) + m1(0,3)*m2(3,1);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0) + m1(1,3)*m2(3,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1) + m1(1,3)*m2(3,1);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0) + m1(2,3)*m2(3,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1) + m1(2,3)*m2(3,1);

    o(3,0) = m1(3,0)*m2(0,0) + m1(3,1)*m2(1,0) + m1(3,2)*m2(2,0) + m1(3,3)*m2(3,0);
    o(3,1) = m1(3,0)*m2(0,1) + m1(3,1)*m2(1,1) + m1(3,2)*m2(2,1) + m1(3,3)*m2(3,1);

    return o;
}

template <typename T, typename U>
constexpr Mat4x3<T> operator*(const Mat4<T>& m1, const Mat4x3<U>& m2) {
    Mat4x3<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0) + m1(0,3)*m2(3,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1) + m1(0,3)*m2(3,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2) + m1(0,3)*m2(3,2);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0) + m1(1,3)*m2(3,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1) + m1(1,3)*m2(3,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2) + m1(1,3)*m2(3,2);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0) + m1(2,3)*m2(3,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1) + m1(2,3)*m2(3,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2) + m1(2,2)*m2(2,2) + m1(2,3)*m2(3,2);

    o(3,0) = m1(3,0)*m2(0,0) + m1(3,1)*m2(1,0) + m1(3,2)*m2(2,0) + m1(3,3)*m2(3,0);
    o(3,1) = m1(3,0)*m2(0,1) + m1(3,1)*m2(1,1) + m1(3,2)*m2(2,1) + m1(3,3)*m2(3,1);
    o(3,2) = m1(3,0)*m2(0,2) + m1(3,1)*m2(1,2) + m1(3,2)*m2(2,2) + m1(3,3)*m2(3,2);

    return o;
}

template <typename T, typename U>
constexpr Mat4<T> operator*(const Mat4<T>& m1, const Mat4<U>& m2) {
    Mat4<T> o;

    o(0,0) = m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) + m1(0,2)*m2(2,0) + m1(0,3)*m2(3,0);
    o(0,1) = m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1) + m1(0,2)*m2(2,1) + m1(0,3)*m2(3,1);
    o(0,2) = m1(0,0)*m2(0,2) + m1(0,1)*m2(1,2) + m1(0,2)*m2(2,2) + m1(0,3)*m2(3,2);
    o(0,3) = m1(0,0)*m2(0,3) + m1(0,1)*m2(1,3) + m1(0,2)*m2(2,3) + m1(0,3)*m2(3,3);

    o(1,0) = m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) + m1(1,2)*m2(2,0) + m1(1,3)*m2(3,0);
    o(1,1) = m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1) + m1(1,2)*m2(2,1) + m1(1,3)*m2(3,1);
    o(1,2) = m1(1,0)*m2(0,2) + m1(1,1)*m2(1,2) + m1(1,2)*m2(2,2) + m1(1,3)*m2(3,2);
    o(1,3) = m1(1,0)*m2(0,3) + m1(1,1)*m2(1,3) + m1(1,2)*m2(2,3) + m1(1,3)*m2(3,3);

    o(2,0) = m1(2,0)*m2(0,0) + m1(2,1)*m2(1,0) + m1(2,2)*m2(2,0) + m1(2,3)*m2(3,0);
    o(2,1) = m1(2,0)*m2(0,1) + m1(2,1)*m2(1,1) + m1(2,2)*m2(2,1) + m1(2,3)*m2(3,1);
    o(2,2) = m1(2,0)*m2(0,2) + m1(2,1)*m2(1,2) + m1(2,2)*m2(2,2) + m1(2,3)*m2(3,2);
    o(2,3) = m1(2,0)*m2(0,3) + m1(2,1)*m2(1,3) + m1(2,2)*m2(2,3) + m1(2,3)*m2(3,3);

    o(3,0) = m1(3,0)*m2(0,0) + m1(3,1)*m2(1,0) + m1(3,2)*m2(2,0) + m1(3,3)*m2(3,0);
    o(3,1) = m1(3,0)*m2(0,1) + m1(3,1)*m2(1,1) + m1(3,2)*m2(2,1) + m1(3,3)*m2(3,1);
    o(3,2) = m1(3,0)*m2(0,2) + m1(3,1)*m2(1,2) + m1(3,2)*m2(2,2) + m1(3,3)*m2(3,2);
    o(3,3) = m1(3,0)*m2(0,3) + m1(3,1)*m2(1,3) + m1(3,2)*m2(2,3) + m1(3,3)*m2(3,3);

    return o;
}

// ---------------------------------------------------------------- Addition
template <typename T, typename U>
constexpr Vec2<T> operator+(const Vec2<T>& v1, const Vec2<U>& v2){
    return {v1(0) + v2(0), v1(1) + v2(1)};
}

template <typename T, typename U>
constexpr Vec3<T> operator+(const Vec3<T>& v1, const Vec3<U>& v2) {
    return {v1(0) + v2(0), v1(1) + v2(1), v1(2) + v2(2)};
}

template <typename T, typename U>
constexpr Vec4<T> operator+(const Vec4<T>& v1, const Vec4<U>& v2) {
    return {v1(0) + v2(0), v1(1) + v2(1), v1(2) + v2(2), v1(3) + v2(3)};
}

template <typename T, typename U>
constexpr Mat2<T> operator+(const Mat2<T>& m1, const Mat2<U>& m2) {
    Mat2<T> o;
    o(0) = m1(0) + m2(0);  o(1) = m1(1) + m2(1);
    o(2) = m1(2) + m2(2);  o(3) = m1(3) + m2(3);
    return o;
}

template <typename T, typename U>
constexpr Mat3<T> operator+(const Mat3<T>& m1, const Mat3<U>& m2) {
    Mat3<T> o;
    o(0) = m1(0) + m2(0);  o(1) = m1(1) + m2(1);  o(2) = m1(2) + m2(2);
    o(3) = m1(3) + m2(3);  o(4) = m1(4) + m2(4);  o(5) = m1(5) + m2(5);
    o(6) = m1(6) + m2(6);  o(7) = m1(7) + m2(7);  o(8) = m1(8) + m2(8);
    return o;
}

template <typename T, typename U>
constexpr Mat4<T> operator+(const Mat4<T>& m1, const Mat4<U>& m2) {
    Mat4<T> o;
    o(0)  = m1(0)  + m2(0);   o(1)  = m1(1)  + m2(1);   o(2)  = m1(2)  + m2(2);   o(3)  = m1(3)  + m2(3);
    o(4)  = m1(4)  + m2(4);   o(5)  = m1(5)  + m2(5);   o(6)  = m1(6)  + m2(6);   o(7)  = m1(7)  + m2(7);
    o(8)  = m1(8)  + m2(8);   o(9)  = m1(9)  + m2(9);   o(10) = m1(10) + m2(10);  o(11) = m1(11) + m2(11);
    o(12) = m1(12) + m2(12);  o(13) = m1(13) + m2(13);  o(14) = m1(14) + m2(14);  o(15) = m1(15) + m2(15);
    return o;
}

template <typename T, typename U>
constexpr Mat2x3<T> operator+(const Mat2x3<T>& m1, const Mat2x3<U>& m2) {
    Mat2x3<T> o;
    o(0) = m1(0) + m2(0);  o(1) = m1(1) + m2(1);  o(2) = m1(2) + m2(2);
    o(3) = m1(3) + m2(3);  o(4) = m1(4) + m2(4);  o(5) = m1(5) + m2(5);
    return o;
}

template <typename T, typename U>
constexpr Mat3x2<T> operator+(const Mat3x2<T>& m1, const Mat3x2<U>& m2) {
    Mat3x2<T> o;
    o(0) = m1(0) + m2(0);  o(1) = m1(1) + m2(1);  o(2) = m1(2) + m2(2);
    o(3) = m1(3) + m2(3);  o(4) = m1(4) + m2(4);  o(5) = m1(5) + m2(5);
    return o;
}

template <typename T, typename U>
constexpr Mat2x4<T> operator+(const Mat2x4<T>& m1, const Mat2x4<U>& m2) {
    Mat2x4<T> o;
    o(0) = m1(0) + m2(0);  o(1) = m1(1) + m2(1);  o(2) = m1(2) + m2(2);  o(3) = m1(3) + m2(3);
    o(4) = m1(4) + m2(4);  o(5) = m1(5) + m2(5);  o(6) = m1(6) + m2(6);  o(7) = m1(7) + m2(7);
    return o;
}

template <typename T, typename U>
constexpr Mat4x2<T> operator+(const Mat4x2<T>& m1, const Mat4x2<U>& m2) {
    Mat4x2<T> o;
    o(0) = m1(0) + m2(0);  o(1) = m1(1) + m2(1);
    o(2) = m1(2) + m2(2);  o(3) = m1(3) + m2(3);
    o(4) = m1(4) + m2(4);  o(5) = m1(5) + m2(5);
    o(6) = m1(6) + m2(6);  o(7) = m1(7) + m2(7);
    return o;
}

template <typename T, typename U>
constexpr Mat3x4<T> operator+(const Mat3x4<T>& m1, const Mat3x4<U>& m2) {
    Mat3x4<T> o;
    o(0)  = m1(0)  + m2(0);   o(1)  = m1(1)  + m2(1);   o(2)  = m1(2)  + m2(2);   o(3)  = m1(3)  + m2(3);
    o(4)  = m1(4)  + m2(4);   o(5)  = m1(5)  + m2(5);   o(6)  = m1(6)  + m2(6);   o(7)  = m1(7)  + m2(7);
    o(8)  = m1(8)  + m2(8);   o(9)  = m1(9)  + m2(9);   o(10) = m1(10) + m2(10);  o(11) = m1(11) + m2(11);
    return o;
}

template <typename T, typename U>
constexpr Mat4x3<T> operator+(const Mat4x3<T>& m1, const Mat4x3<U>& m2) {
    Mat4x3<T> o;
    o(0)  = m1(0)  + m2(0);   o(1)  = m1(1)  + m2(1);   o(2)  = m1(2)  + m2(2);
    o(3)  = m1(3)  + m2(3);   o(4)  = m1(4)  + m2(4);   o(5)  = m1(5)  + m2(5);
    o(6)  = m1(6)  + m2(6);   o(7)  = m1(7)  + m2(7);   o(8)  = m1(8)  + m2(8);
    o(9)  = m1(9)  + m2(9);   o(10) = m1(10) + m2(10);  o(11) = m1(11) + m2(11);
    return o;
}

// ---------------------------------------------------------------- Matrix * Scalar
template <typename T, typename U>
constexpr Mat2<T> operator*(const Mat2<T>& m, U scalar) {
    Mat2<T> o;
    o(0) = m(0) * scalar;  o(1) = m(1) * scalar;
    o(2) = m(2) * scalar;  o(3) = m(3) * scalar;
    return o;
}

template <typename T, typename U>
constexpr Mat3<T> operator*(const Mat3<T>& m, U scalar) {
    Mat3<T> o;
    o(0) = m(0) * scalar;  o(1) = m(1) * scalar;  o(2) = m(2) * scalar;
    o(3) = m(3) * scalar;  o(4) = m(4) * scalar;  o(5) = m(5) * scalar;
    o(6) = m(6) * scalar;  o(7) = m(7) * scalar;  o(8) = m(8) * scalar;
    return o;
}

template <typename T, typename U>
constexpr Mat4<T> operator*(const Mat4<T>& m, U scalar) {
    Mat4<T> o;
    o(0)  = m(0)  * scalar;  o(1)  = m(1)  * scalar;  o(2)  = m(2)  * scalar;  o(3)  = m(3)  * scalar;
    o(4)  = m(4)  * scalar;  o(5)  = m(5)  * scalar;  o(6)  = m(6)  * scalar;  o(7)  = m(7)  * scalar;
    o(8)  = m(8)  * scalar;  o(9)  = m(9)  * scalar;  o(10) = m(10) * scalar;  o(11) = m(11) * scalar;
    o(12) = m(12) * scalar;  o(13) = m(13) * scalar;  o(14) = m(14) * scalar;  o(15) = m(15) * scalar;
    return o;
}

template <typename T, typename U>
constexpr Mat2x3<T> operator*(const Mat2x3<T>& m, U scalar) {
    Mat2x3<T> o;
    o(0) = m(0) * scalar;  o(1) = m(1) * scalar;  o(2) = m(2) * scalar;
    o(3) = m(3) * scalar;  o(4) = m(4) * scalar;  o(5) = m(5) * scalar;
    return o;
}

template <typename T, typename U>
constexpr Mat3x2<T> operator*(const Mat3x2<T>& m, U scalar) {
    Mat3x2<T> o;
    o(0) = m(0) * scalar;  o(1) = m(1) * scalar;  o(2) = m(2) * scalar;
    o(3) = m(3) * scalar;  o(4) = m(4) * scalar;  o(5) = m(5) * scalar;
    return o;
}

template <typename T, typename U>
constexpr Mat2x4<T> operator*(const Mat2x4<T>& m, U scalar) {
    Mat2x4<T> o;
    o(0) = m(0) * scalar;  o(1) = m(1) * scalar;  o(2) = m(2) * scalar;  o(3) = m(3) * scalar;
    o(4) = m(4) * scalar;  o(5) = m(5) * scalar;  o(6) = m(6) * scalar;  o(7) = m(7) * scalar;
    return o;
}

template <typename T, typename U>
constexpr Mat4x2<T> operator*(const Mat4x2<T>& m, U scalar) {
    Mat4x2<T> o;
    o(0) = m(0) * scalar;  o(1) = m(1) * scalar;
    o(2) = m(2) * scalar;  o(3) = m(3) * scalar;
    o(4) = m(4) * scalar;  o(5) = m(5) * scalar;
    o(6) = m(6) * scalar;  o(7) = m(7) * scalar;
    return o;
}

template <typename T, typename U>
constexpr Mat3x4<T> operator*(const Mat3x4<T>& m, U scalar) {
    Mat3x4<T> o;
    o(0)  = m(0)  * scalar;  o(1)  = m(1)  * scalar;  o(2)  = m(2)  * scalar;  o(3)  = m(3)  * scalar;
    o(4)  = m(4)  * scalar;  o(5)  = m(5)  * scalar;  o(6)  = m(6)  * scalar;  o(7)  = m(7)  * scalar;
    o(8)  = m(8)  * scalar;  o(9)  = m(9)  * scalar;  o(10) = m(10) * scalar;  o(11) = m(11) * scalar;
    return o;
}

template <typename T, typename U>
constexpr Mat4x3<T> operator*(const Mat4x3<T>& m, U scalar) {
    Mat4x3<T> o;
    o(0)  = m(0)  * scalar;  o(1)  = m(1)  * scalar;  o(2)  = m(2)  * scalar;
    o(3)  = m(3)  * scalar;  o(4)  = m(4)  * scalar;  o(5)  = m(5)  * scalar;
    o(6)  = m(6)  * scalar;  o(7)  = m(7)  * scalar;  o(8)  = m(8)  * scalar;
    o(9)  = m(9)  * scalar;  o(10) = m(10) * scalar;  o(11) = m(11) * scalar;
    return o;
}

// ---------------------------------------------------------------- Scalar * Matrix
template <typename T, typename U> constexpr Mat2<T>   operator*(U scalar, const Mat2<T>&   m) { return m * scalar; }
template <typename T, typename U> constexpr Mat3<T>   operator*(U scalar, const Mat3<T>&   m) { return m * scalar; }
template <typename T, typename U> constexpr Mat4<T>   operator*(U scalar, const Mat4<T>&   m) { return m * scalar; }
template <typename T, typename U> constexpr Mat2x3<T> operator*(U scalar, const Mat2x3<T>& m) { return m * scalar; }
template <typename T, typename U> constexpr Mat3x2<T> operator*(U scalar, const Mat3x2<T>& m) { return m * scalar; }
template <typename T, typename U> constexpr Mat2x4<T> operator*(U scalar, const Mat2x4<T>& m) { return m * scalar; }
template <typename T, typename U> constexpr Mat4x2<T> operator*(U scalar, const Mat4x2<T>& m) { return m * scalar; }
template <typename T, typename U> constexpr Mat3x4<T> operator*(U scalar, const Mat3x4<T>& m) { return m * scalar; }
template <typename T, typename U> constexpr Mat4x3<T> operator*(U scalar, const Mat4x3<T>& m) { return m * scalar; }

// ---------------------------------------------------------------- Identity / Inverse
template <typename T>
constexpr Mat3<T> Identity3x3() {
    Mat3<T> I;
    I(0,0) = T(1);
    I(1,1) = T(1);
    I(2,2) = T(1);
    return I;
}

template <typename T>
constexpr Mat4<T> Identity4x4() {
    Mat4<T> I;
    I(0,0) = T(1);
    I(1,1) = T(1);
    I(2,2) = T(1);
    I(3,3) = T(1);
    return I;
}

template <typename T>
constexpr Mat2<T> inverse(const Mat2<T>& m) {
    const T inv_det = T(1) / (m(0,0)*m(1,1) - m(0,1)*m(1,0));
    return Mat2<T>{
         m(1,1) * inv_det, -m(0,1) * inv_det,
        -m(1,0) * inv_det,  m(0,0) * inv_det
    };
}

template <typename T>
constexpr Mat3<T> inverse(const Mat3<T>& m) {
    const T det =
        m(0,0) * (m(1,1) * m(2,2) - m(1,2) * m(2,1))
      - m(0,1) * (m(1,0) * m(2,2) - m(1,2) * m(2,0))
      + m(0,2) * (m(1,0) * m(2,1) - m(1,1) * m(2,0));
    const T inv_det = T(1) / det;

    return Mat3<T>{
        (m(1,1) * m(2,2) - m(1,2) * m(2,1)) * inv_det,
        (m(0,2) * m(2,1) - m(0,1) * m(2,2)) * inv_det,
        (m(0,1) * m(1,2) - m(0,2) * m(1,1)) * inv_det,

        (m(1,2) * m(2,0) - m(1,0) * m(2,2)) * inv_det,
        (m(0,0) * m(2,2) - m(0,2) * m(2,0)) * inv_det,
        (m(0,2) * m(1,0) - m(0,0) * m(1,2)) * inv_det,

        (m(1,0) * m(2,1) - m(1,1) * m(2,0)) * inv_det,
        (m(0,1) * m(2,0) - m(0,0) * m(2,1)) * inv_det,
        (m(0,0) * m(1,1) - m(0,1) * m(1,0)) * inv_det
    };
}

// ---------------------------------------------------------------- Projection / IO
// Orthographic projection. Written row-major (brace-init), stored column-major,
// so the storage can be uploaded directly with transpose = GL_FALSE.
static inline Mat4f transform_matrix(float left, float right, float bottom, float top, float near, float far){
    float rl = right - left;
    float tb = top - bottom;
    float fn = far - near;

    return Mat4f({
         2.0f / rl,  0.0f,        0.0f,        -(right + left) / rl,
         0.0f,       2.0f / tb,   0.0f,        -(top + bottom) / tb,
         0.0f,       0.0f,       -2.0f / fn,   -(far + near) / fn,
         0.0f,       0.0f,        0.0f,         1.0f
    });
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Mat4<T>& mat)
{
    os << "[ " << mat(0,0) << ", " << mat(0,1) << ", " << mat(0,2) << ", " << mat(0,3) << " ]\n";
    os << "[ " << mat(1,0) << ", " << mat(1,1) << ", " << mat(1,2) << ", " << mat(1,3) << " ]\n";
    os << "[ " << mat(2,0) << ", " << mat(2,1) << ", " << mat(2,2) << ", " << mat(2,3) << " ]\n";
    os << "[ " << mat(3,0) << ", " << mat(3,1) << ", " << mat(3,2) << ", " << mat(3,3) << " ]";
    return os;
}

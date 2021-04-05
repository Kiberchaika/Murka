#pragma once

#ifdef MURKA_JUCE


//==============================================================================
/**
	A 4x4 3D transformation matrix.

	@see Vector3D, Quaternion, AffineTransform

	@tags{OpenGL}
*/
template <typename Type>
class MurMatrix
{
public:
	/** Creates an identity matrix. */
	MurMatrix() noexcept
	{
		mat[0] = Type(1); mat[1] = 0;        mat[2] = 0;         mat[3] = 0;
		mat[4] = 0;        mat[5] = Type(1); mat[6] = 0;         mat[7] = 0;
		mat[8] = 0;        mat[9] = 0;        mat[10] = Type(1);  mat[11] = 0;
		mat[12] = 0;        mat[13] = 0;        mat[14] = 0;         mat[15] = Type(1);
	}

	/** Creates a copy of another matrix. */
	MurMatrix(const MurMatrix& other) noexcept
	{
		memcpy(mat, other.mat, sizeof(mat));
	}

	/** Copies another matrix. */
	MurMatrix& operator= (const MurMatrix& other) noexcept
	{
		memcpy(mat, other.mat, sizeof(mat));
		return *this;
	}

	/** Creates a matrix from its raw 4x4 values. */
	MurMatrix(Type m00, Type m10, Type m20, Type m30,
		Type m01, Type m11, Type m21, Type m31,
		Type m02, Type m12, Type m22, Type m32,
		Type m03, Type m13, Type m23, Type m33) noexcept
	{
		mat[0] = m00;  mat[1] = m10;  mat[2] = m20;  mat[3] = m30;
		mat[4] = m01;  mat[5] = m11;  mat[6] = m21;  mat[7] = m31;
		mat[8] = m02;  mat[9] = m12;  mat[10] = m22;  mat[11] = m32;
		mat[12] = m03;  mat[13] = m13;  mat[14] = m23;  mat[15] = m33;
	}

	/** Creates a matrix from an array of 16 raw values. */
	MurMatrix(const Type* values) noexcept
	{
		memcpy(mat, values, sizeof(mat));
	}

	/** Creates a matrix from a 2D affine transform. */
	MurMatrix(const AffineTransform& transform) noexcept
	{
		mat[0] = transform.mat00;  mat[1] = transform.mat10;  mat[2] = 0;         mat[3] = 0;
		mat[4] = transform.mat01;  mat[5] = transform.mat11;  mat[6] = 0;         mat[7] = 0;
		mat[8] = 0;                mat[9] = 0;                mat[10] = Type(1);  mat[11] = 0;
		mat[12] = transform.mat02;  mat[13] = transform.mat12;  mat[14] = 0;         mat[15] = Type(1);
	}

	/** Creates a matrix from a 3D vector translation. */
	MurMatrix(Vector3D<Type> vector) noexcept
	{
		mat[0] = Type(1); mat[1] = 0;        mat[2] = 0;         mat[3] = 0;
		mat[4] = 0;        mat[5] = Type(1); mat[6] = 0;         mat[7] = 0;
		mat[8] = 0;        mat[9] = 0;        mat[10] = Type(1);  mat[11] = 0;
		mat[12] = vector.x; mat[13] = vector.y; mat[14] = vector.z;  mat[15] = Type(1);
	}

	/** Returns a new matrix from the given frustum values. */
	static MurMatrix fromFrustum(Type left, Type right, Type bottom, Type top, Type nearDistance, Type farDistance) noexcept
	{
		return{ (Type(2) * nearDistance) / (right - left), 0, 0, 0,
				  0, (Type(2) * nearDistance) / (top - bottom), 0, 0,
				  (right + left) / (right - left), (top + bottom) / (top - bottom), -(farDistance + nearDistance) / (farDistance - nearDistance), Type(-1),
				  0, 0, -(Type(2) * farDistance * nearDistance) / (farDistance - nearDistance), 0 };
	}

	/** Returns a new matrix from the given ortho values. */
	static MurMatrix fromOrtho(Type left, Type right, Type bottom, Type top, Type nearDistance, Type farDistance) noexcept
	{
		return{ Type(2) / (right - left), 0, 0, 0,
				 0, Type(2) / (top - bottom), 0, 0,
				 0, 0, -Type(1) / (farDistance - nearDistance), 0,
				 -(right + left) / (right - left), -(top + bottom) / (top - bottom), -nearDistance / (farDistance - nearDistance), 0 };
	}

	/** Returns a new matrix from the given perspective values. */
	static MurMatrix fromPerspective(Type fieldOfViewY, Type aspect, Type nearDistance, Type farDistance) noexcept
	{
		const Type tanHalfFovY = std::tan(fieldOfViewY / Type(2));

		return{ Type(1) / (aspect * tanHalfFovY), 0, 0, 0,
				 0, Type(1) / tanHalfFovY, 0, 0,
				 0, 0, -(farDistance + nearDistance) / (farDistance - nearDistance), -Type(1),
				 0, 0, -(Type(2) * farDistance * nearDistance) / (farDistance - nearDistance), 0 };
	}

	/** Returns a new matrix from the given perspective field of view values. */
	static MurMatrix fromPerspectiveFieldOfView(Type fieldOfView, Type width, Type height, Type nearDistance, Type farDistance) noexcept
	{
		jassert(width > Type(0));
		jassert(height > Type(0));
		jassert(fieldOfView > Type(0));

		const auto rad = fieldOfView;
		const auto h = std::cos(static_cast<Type> (0.5) * rad) / std::sin(static_cast<Type> (0.5) * rad);
		const auto w = h * height / width;

		return{ w, 0, 0, 0,
				 0, h, 0, 0,
				 0, 0, (farDistance + nearDistance) / (farDistance - nearDistance), -Type(1),
				 0, 0, -(farDistance * nearDistance) / (farDistance - nearDistance), 0 };
	}

	/** Returns a new look-at matrix from the provided vectors. */
	static MurMatrix fromLookAt(Vector3D<Type> eye, Vector3D<Type> center, Vector3D<Type> up) noexcept
	{
		const auto f = Vector3D<Type>(center - eye).normalised();
		const auto s = Vector3D<Type>(f ^ up).normalised();
		const auto u = s ^ f;

		return{ s.x, s.y, s.z, 0,
				u.x, u.y, u.z, 0,
				-f.x, -f.y, -f.z, 0,
				-(s * eye),  -(u * eye), (f * eye), Type(1) };
	}

	/** Returns a matrix which will apply a rotation through the X, Y, and Z angles specified by a vector. */
	static MurMatrix rotation(Vector3D<Type> eulerAngleRadians) noexcept
	{
		auto cx = std::cos(eulerAngleRadians.x), sx = std::sin(eulerAngleRadians.x),
			cy = std::cos(eulerAngleRadians.y), sy = std::sin(eulerAngleRadians.y),
			cz = std::cos(eulerAngleRadians.z), sz = std::sin(eulerAngleRadians.z);

		return{ (cy * cz) + (sx * sy * sz), cx * sz, (cy * sx * sz) - (cz * sy), 0,
				 (cz * sx * sy) - (cy * sz), cx * cz, (cy * cz * sx) + (sy * sz), 0,
				 cx * sy, -sx, cx * cy, 0,
				 0, 0, 0, Type(1) };
	}

	/** Returns a version of this matrix rotated by the provided vector. */
	MurMatrix rotated(Vector3D<Type> eulerAngleRadians) noexcept
	{
		return *this * rotation(eulerAngleRadians);
	}

	/** Rotates this matrix by the provided vector. */
	void rotate(Vector3D<Type> eulerAngleRadians) noexcept
	{
		*this = rotated(eulerAngleRadians);
	}

	/** Returns a matrix which will apply a scale specified by a vector. */
	MurMatrix scaled(Vector3D<Type> scalar) noexcept
	{
		return{ mat[0] * scalar.x, mat[1] * scalar.x, mat[2] * scalar.x, mat[3] * scalar.x,
				 mat[4] * scalar.x, mat[5] * scalar.y, mat[6] * scalar.y, mat[7] * scalar.y,
				 mat[8] * scalar.z, mat[9] * scalar.z, mat[10] * scalar.z, mat[11] * scalar.z,
				 mat[12], mat[13], mat[14], mat[15] };
	}

	MurMatrix transposed() 
	{
		MurMatrix m;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m.mat[i * 4 + j] = this->mat[j * 4 + i];
			}
		}
		return m;
	}

	/** Scales this matrix by the provided vector. */
	void scale(Vector3D<Type> scalar) noexcept
	{
		*this = scaled(scalar);
	}

	/** Returns a translation matrix. */
	static MurMatrix translation(Vector3D<Type> delta) noexcept
	{
		return{ Type(1), 0, 0, 0,
				 0, Type(1), 0, 0,
				 0, 0, Type(1), 0,
				 delta.x, delta.y, delta.z, Type(1) };
	}

	/** Returns a matrix which will apply a translation specified by the provided vector. */
	MurMatrix translated(Vector3D<Type> delta) noexcept
	{
		return{ mat[0], mat[1], mat[2], mat[3] * delta.x,
				 mat[4], mat[5], mat[6], mat[7] * delta.y,
				 mat[8], mat[9], mat[10], mat[11] * delta.z,
				 mat[12] * delta.x, mat[13] * delta.y, mat[14] * delta.z, mat[15] };
	}

	/** Translates this matrix by the provided vector. */
	void translate(Vector3D<Type> delta) noexcept
	{
		*this = translated(delta);
	}

	/** Multiplies this matrix by another. */
	MurMatrix& operator*= (const MurMatrix& other) noexcept
	{
		return *this = *this * other;
	}

	/** Multiplies this matrix by another, and returns the result. */
	MurMatrix operator* (const MurMatrix& other) const noexcept
	{
		auto&& m2 = other.mat;

		return{ mat[0] * m2[0] + mat[1] * m2[4] + mat[2] * m2[8] + mat[3] * m2[12],
				 mat[0] * m2[1] + mat[1] * m2[5] + mat[2] * m2[9] + mat[3] * m2[13],
				 mat[0] * m2[2] + mat[1] * m2[6] + mat[2] * m2[10] + mat[3] * m2[14],
				 mat[0] * m2[3] + mat[1] * m2[7] + mat[2] * m2[11] + mat[3] * m2[15],
				 mat[4] * m2[0] + mat[5] * m2[4] + mat[6] * m2[8] + mat[7] * m2[12],
				 mat[4] * m2[1] + mat[5] * m2[5] + mat[6] * m2[9] + mat[7] * m2[13],
				 mat[4] * m2[2] + mat[5] * m2[6] + mat[6] * m2[10] + mat[7] * m2[14],
				 mat[4] * m2[3] + mat[5] * m2[7] + mat[6] * m2[11] + mat[7] * m2[15],
				 mat[8] * m2[0] + mat[9] * m2[4] + mat[10] * m2[8] + mat[11] * m2[12],
				 mat[8] * m2[1] + mat[9] * m2[5] + mat[10] * m2[9] + mat[11] * m2[13],
				 mat[8] * m2[2] + mat[9] * m2[6] + mat[10] * m2[10] + mat[11] * m2[14],
				 mat[8] * m2[3] + mat[9] * m2[7] + mat[10] * m2[11] + mat[11] * m2[15],
				 mat[12] * m2[0] + mat[13] * m2[4] + mat[14] * m2[8] + mat[15] * m2[12],
				 mat[12] * m2[1] + mat[13] * m2[5] + mat[14] * m2[9] + mat[15] * m2[13],
				 mat[12] * m2[2] + mat[13] * m2[6] + mat[14] * m2[10] + mat[15] * m2[14],
				 mat[12] * m2[3] + mat[13] * m2[7] + mat[14] * m2[11] + mat[15] * m2[15] };
	}

	/** The 4x4 matrix values. These are stored in the standard OpenGL order. */
	Type mat[16];
};
#endif

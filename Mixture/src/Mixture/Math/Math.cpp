#include "mxpch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Mixture::Math {
	bool decomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale) {
		// From glm::decompose in matrix_decompose.inl
		glm::mat4 localMatrix(transform);
		#define EPSILON_NOT_EQUAL(x, y) glm::epsilonNotEqual<float>(localMatrix[x][y], 0.0f, glm::epsilon<float>())

		// Normalize the matrix.
		if (glm::epsilonEqual<float>(localMatrix[3][3], 0.0f, glm::epsilon<float>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (EPSILON_NOT_EQUAL(0, 3) || EPSILON_NOT_EQUAL(1, 3) || EPSILON_NOT_EQUAL(2, 3)) {
			// Clear the perspective partition
			localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = 0.0f;
			localMatrix[3][3] = 1.0f;
		}

		// Next take care of translation (easy).
		translation = glm::vec3(localMatrix[3]);
		localMatrix[3] = glm::vec4(0, 0, 0, localMatrix[3].w);

		glm::vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (glm::length_t i = 0; i < 3; ++i)
			for (glm::length_t j = 0; j < 3; ++j)
				Row[i][j] = localMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = glm::length(Row[0]);
		Row[0] = glm::detail::scale(Row[0], 1.0f);
		scale.y = glm::length(Row[1]);
		Row[1] = glm::detail::scale(Row[1], 1.0f);
		scale.z = glm::length(Row[2]);
		Row[2] = glm::detail::scale(Row[2], 1.0f);

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = glm::cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (glm::dot(Row[0], Pdum3) < 0) {
			for (glm::length_t i = 0; i < 3; i++) {
				scale[i] *= -1.0f;
				Row[i] *= 1.0f;
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		} else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}

		return true;
	}
}



#include "tools.h"
#include "gl_utils.h"

#define PI 3.14159265359

bool load_mesh(const char *file_name, GLuint *vao, int *point_count){
	const aiScene *scene = aiImportFile(file_name, aiProcess_Triangulate);
	if(!scene){
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return false;
	}
	printf(" %i animations\n", scene->mNumAnimations);
	printf(" %i cameras\n", scene->mNumCameras);
	printf(" %i lights\n", scene->mNumLights);
	printf(" %i materials\n", scene->mNumMaterials);
	printf(" %i meshes\n", scene->mNumMeshes);
	printf(" %i textures\n", scene->mNumTextures);
	
	const aiMesh *mesh = scene->mMeshes[0];
	printf(" %i vertices in mesh[0]\n", mesh->mNumVertices);
	
	*point_count = mesh->mNumVertices;
	
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);
	
	GLfloat* points = NULL;
	GLfloat* normals = NULL;
	GLfloat* texcoords = NULL;

	if(mesh->HasPositions()){
		points = (GLfloat*) malloc(*point_count * 3 * sizeof(GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D *vp = &(mesh->mVertices[i]);
			points[i * 3] = (GLfloat)vp->x;
			points[i * 3 + 1] = (GLfloat)vp->y;
			points[i * 3 + 2] = (GLfloat)vp->z;
		}
	}

	if(mesh->HasNormals()){
		normals = (GLfloat*) malloc(*point_count * 3 * sizeof(GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vn = &(mesh->mNormals[i]);
			normals[i * 3] = (GLfloat)vn->x;
			normals[i * 3 + 1] = (GLfloat)vn->y;
			normals[i * 3 + 2] = (GLfloat)vn->z;
		}
	}

	if(mesh->HasTextureCoords(0)){
		texcoords = (GLfloat*) malloc(*point_count * 2 * sizeof(GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
			texcoords[i * 2] = (GLfloat)vt->x;
			texcoords[i * 2 + 1] = (GLfloat)vt->y;
		}
	}
	
	if (mesh->HasPositions()){
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(
			GL_ARRAY_BUFFER,
			3 * *point_count * sizeof(GLfloat),
			points,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		free(points);
	}

	if(mesh->HasNormals()){
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(
			GL_ARRAY_BUFFER,
			3 * *point_count * sizeof(GLfloat),
			normals,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
		free(normals);
	}

	if(mesh->HasTextureCoords(0)){
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(
			GL_ARRAY_BUFFER,
			2 * *point_count * sizeof(GLfloat),
			texcoords,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);
		free(texcoords);
	}

	if (mesh->HasTangentsAndBitangents ()) {
		// NB: could store/print tangents here
	}
	
	aiReleaseImport(scene);
	printf("Mesh loaded\n");
	return true;
}

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "Block.h"
#include "PerlinNoise.h"

class Chunk {
public:
    Chunk();
    Chunk(int x, int z);
    ~Chunk();
    void update(float dt);
    GLuint render();
    void draw(int attribute);
    void createMesh();
    void createLandscape();
    void deleteBuffer();
    glm::vec3 getPosition();
    static const int CHUNK_SIZE = 16;

private:
    GLuint m_vertexbuffer;
    GLuint m_normalbuffer;
    glm::vec3 position;
    Block*** m_pBlocks;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    void createCube(GLfloat x, GLfloat y, GLfloat z);
};

glm::vec3 Chunk::getPosition() { return position; }

void Chunk::deleteBuffer() {
  glDeleteBuffers(1, &m_vertexbuffer);
  glDeleteBuffers(1, &m_normalbuffer); 
}

GLuint Chunk::render() {
  auto vb = &m_vertexbuffer;
	glGenBuffers(1, vb);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
  auto nb = &m_normalbuffer;
	glGenBuffers(1, nb);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
}

void Chunk::draw(int attribute) {
  glEnableVertexAttribArray(attribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
  glVertexAttribPointer(
    attribute,          // attribute. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );
  glDrawArrays(GL_TRIANGLES, 0, (vertices.size()/3)*3);
  glDisableVertexAttribArray(attribute);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
  glVertexAttribPointer(
    1,          // attribute. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );
  glDisableVertexAttribArray(1);
}

Chunk::Chunk() {
    position = glm::vec3(0,0,0);
    m_pBlocks = new Block**[CHUNK_SIZE];
    for(int i = 0; i < CHUNK_SIZE; i++) {
        m_pBlocks[i] = new Block*[CHUNK_SIZE];
        for(int j = 0; j < CHUNK_SIZE; j++) {
            m_pBlocks[i][j] = new Block[CHUNK_SIZE];
        }
    }
}

Chunk::Chunk(int x, int z) {
    position = glm::vec3(x*CHUNK_SIZE, 0, z*CHUNK_SIZE);
    m_pBlocks = new Block**[CHUNK_SIZE];
    for(int i = 0; i < CHUNK_SIZE; i++) {
        m_pBlocks[i] = new Block*[CHUNK_SIZE];
        for(int j = 0; j < CHUNK_SIZE; j++) {
            m_pBlocks[i][j] = new Block[CHUNK_SIZE];
        }
    }
}

Chunk::~Chunk() {
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        for (int j = 0; j < CHUNK_SIZE; ++j) {
            delete [] m_pBlocks[i][j];
        }
        delete [] m_pBlocks[i];
    }
    delete [] m_pBlocks;
}

void Chunk::createMesh() {
    createLandscape();
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if(m_pBlocks[x][y][z].isActive() == false) {
                    continue;
                }
                if ((x > 0 && x < CHUNK_SIZE-1) &&
                    (y > 0 && y < CHUNK_SIZE-1) &&
                    (z > 0 && z < CHUNK_SIZE-1) &&
                    m_pBlocks[x-1][y][z].isActive() &&
                    m_pBlocks[x+1][y][z].isActive() &&
                    m_pBlocks[x][y-1][z].isActive() &&
                    m_pBlocks[x][y+1][z].isActive() &&
                    m_pBlocks[x][y][z-1].isActive() &&
                    m_pBlocks[x][y][z+1].isActive()) {
                      // do nothing
                } else {
                  createCube(x+position.x, y+position.y, z+position.z);
                }
            }
        }
    }
}

void Chunk::createLandscape() {
  PerlinNoise pn(42);
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int z = 0; z < CHUNK_SIZE; z++) {
      double height = pn.noise(x+position.x, z+position.z, 0.8) * (CHUNK_SIZE-1);
      for (int y = 0; y < height; y++) {
        m_pBlocks[x][y][z].setActive(true);
        m_pBlocks[x][y][z].setBlockType(BlockType_Grass);
      }
    }
  }
}

void Chunk::createCube(GLfloat x, GLfloat y, GLfloat z) {
	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	GLfloat mx = float(x)+Block::BLOCK_RENDER_SIZE;
	GLfloat my = float(y)+Block::BLOCK_RENDER_SIZE;
	GLfloat mz = float(z)+Block::BLOCK_RENDER_SIZE;
	GLfloat arr[108] = {
		x,y,z,
		x,y,mz,
		x,my,mz,
		mx,my,z,
		x,y,z,
		x,my,z,
		mx,y,mz,
		x,y,z,
		mx,y,z,
		mx,my,z,
		mx,y,z,
		x,y,z,
		x,y,z,
		x,my,mz,
		x,my,z,
		mx,y,mz,
		x,y,mz,
		x,y,z,
		x,my,mz,
		x,y,mz,
		mx,y,mz,
		mx,my,mz,
		mx,y,z,
		mx,my,z,
		mx,y,z,
		mx,my,mz,
		mx,y,mz,
		mx,my,mz,
		mx,my,z,
		x,my,z,
		mx,my,mz,
		x,my,z,
		x,my,mz,
		mx,my,mz,
		x,my,mz,
		mx,y,mz
	 };
	GLfloat n_arr[] = {
   -1, 0, 0,
   -1, 0, 0,
   -1, 0, 0,
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
    0, -1, 0,
    0, -1, 0,
    0, -1, 0,
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
   -1, 0, 0,
   -1, 0, 0,
   -1, 0, 0,
    0, -1, 0,
    0, -1, 0,
    0, -1, 0,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
   1, 0, 0,
   1, 0, 0,
   1, 0, 0,
   1, 0, 0,
   1, 0, 0,
   1, 0, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1
  };
	vertices.insert(vertices.end(), std::begin(arr), std::end(arr));
  normals.insert(normals.end(), std::begin(n_arr), std::end(n_arr));
}

#include "Chunk.h"
#include <common/controls.hpp>

class ChunkManager {
public:
  ChunkManager() {
    lastx = 0;
    lastz = 0;
    for (int i = lastx-RANGE; i <= lastx+RANGE; i++) {
      for (int j = lastz-RANGE; j <= lastz+RANGE; j++) {
          chunks.push_back(new Chunk(i, j));
      }
    }
    assert(chunks.size() == pow(RANGE*2+1,2));
  }

  ~ChunkManager() {
    for (auto chunk : chunks) {
      delete chunk;
    }
  }

  void renderChunks() {
    for (auto chunk : chunks) {
  		chunk->createMesh();
  		chunk->render();
  	}
  }

  void drawChunks() {
    getAndUpdate();
    //printf("lastx: %d lastz: %d\n", lastx, lastz);
    int minx = lastx-RANGE;
    int maxx = lastx+RANGE;
    int minz = lastz-RANGE;
    int maxz = lastz+RANGE;
    for (size_t i = 0; i < chunks.size(); i++) {
      auto chunkPos = chunks[i]->getPosition();
      int x = (int)chunkPos.x / Chunk::CHUNK_SIZE;
      int z = (int)chunkPos.z / Chunk::CHUNK_SIZE;
      //printf("i: %d | %d, %d\n", (int)i, x, z);
      if ((x >= minx && x <= maxx) && (z >= minz && z <= maxz)) {
        chunks[i]->draw(0);
      } else {
        printf("deleted %d %d\n", x, z);
        chunks[i]->deleteBuffer();
        delete chunks[i];
        std::iter_swap(chunks.begin() + i, chunks.end() - 1);
        chunks.pop_back();
      }
    }
    assert(chunks.size() == pow(RANGE*2+1,2));
    /*if (chunks.size() > 9) {
      printf("greater than 9. size: %d\n", (int)chunks.size());
      printf("minx %d maxx %d minz %d maxz %d\n", minx, maxx, minz, maxz);
      for (auto chunk : chunks) {
        printf("%d %d\n", (int)chunk->getPosition().x / 16, (int)chunk->getPosition().z / 16);
      }
      assert(false);
    } */
  }

  void deleteChunkBuffers() {
    for (auto& chunk : chunks) {
        chunk->deleteBuffer();
    }
  }
private:
    std::vector<Chunk*> chunks;
    static const int RANGE = 3;
    int lastx;
    int lastz;

    void getAndUpdate() {
      auto p = getPosition();
      int newx = floor(p.x) / 16;
      int newz = floor(p.z) / 16;
      if (newx == lastx && newz == lastz) {
        // nothing
      } else {
        int minx = lastx-RANGE;
        int maxx = lastx+RANGE;
        int minz = lastz-RANGE;
        int maxz = lastz+RANGE;
        for (int i = newx-RANGE; i <= newx+RANGE; i++) {
          for (int j = newz-RANGE; j <= newz+RANGE; j++) {
            if ((i >= minx && i <= maxx) && (j >= minz && j <= maxz)) {
              // nothing -- it's already there
            } else {
                chunks.push_back(new Chunk(i, j));
                chunks.back()->createMesh();
  		          chunks.back()->render();
                printf("new block at %d, %d\n", i, j);
            }
          }
        }
      }
      lastx = newx;
      lastz = newz;
    }
};


enum BlockType
{
	BlockType_Default = 0,

	BlockType_Grass,
	BlockType_Dirt,
	BlockType_Water,
	BlockType_Stone,
  BlockType_Wood,
  BlockType_Sand,
  BlockType_NumTypes,
};

class Block {
public:
    Block() {
      m_active = false;
      m_blockType = BlockType_Default;
    }
    ~Block() = default;

    bool isActive() { return m_active; }
    void setActive(bool active) { m_active = active; }
		void setBlockType(BlockType b) { m_blockType = b; }

    static const int BLOCK_RENDER_SIZE = 1;
private:
    bool m_active;
    BlockType m_blockType;
};

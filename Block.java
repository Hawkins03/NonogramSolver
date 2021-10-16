package NonogramSolver;

/**
 * A single line of blocks in a nonogram
 *
 * @author Hawkins Peterson
 * @version 10.16.21
 */
public class Block {
    protected final int LENGTH; //the length of the line of blocks
    protected int knownStartPos = -1; //the first position where the block MUST exist 
    protected int knownEndPos = -1; // the last position where the block MUST exist

    /**
     * constructor for the block class (I don't have any guarenteed starting point when constructing the object)
     * @param LENGTH the length of the block (which obviously can't change in a game)
     */
    public Block(int LENGTH) {
        this.LENGTH = LENGTH;
    }

    /**
     * sets the start point and the end point as we know them
     * @param startPos the first position where the block is guarenteed to be
     * @param endPos the last position where the block is guarenteed to be
     */
    public void setPos(int startPos, int endPos) {
        knownStartPos = startPos;
        knownEndPos = startPos + endPos;
    }

    /**
     * sets the start and ending positions to a single point
     * @param pos where the block is guarenteed to be
     */
    public void setPos(int pos) {
        knownStartPos = pos;
        knownEndPos = pos;
    }

    /**
     * Returns a string version of the class
     * @return a string version of the class
     */
    @Override
    public String toString(){
        return "Block{LENGTH=" + LENGTH + ",startPos=" + knownStartPos + ",endPos=" + knownEndPos + "}";
    }
}

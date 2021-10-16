package NonogramSolver;
import NonogramSolver.*;
/**
 * a single row / column in a nongram "board"
 * @author Hawkins Peterson
 * @version 10.16.21
 */
public class Row {
    protected Block[] blocks; //the array of blocks that this array will hold
    protected double[] probabilities; //the probability that a space will be filled or not
    protected int[] positions; //an integer version of probabilities 0 = not, 1 = maybe, 2 = guarenteed
    protected int blockLength = 0; //the sum of all block lengths + the number of blocks - 1
    protected final int LENGTH; // the total length of the row

    public Row(int LENGTH, int[] blockLengths) {
        blocks = new Block[blockLengths.length];
        for (int i = 0; i < blockLengths.length; i++) {
            blocks[i] = new Block(blockLengths[i]);
            blockLength += blockLengths[i];
        }
        blockLength += blockLengths.length - 1;

        this.LENGTH = LENGTH;
        probabilities = new double[LENGTH + 2];
        positions = new int[LENGTH + 2];
    }

    public void initialSolve() {
        int guarenteedLength = LENGTH - blockLength;
        int itterator = 1;
        for (Block i : blocks) {
            if (i.LENGTH > guarenteedLength) {
                i.setPos(itterator + i.LENGTH - guarenteedLength, itterator + i.LENGTH);
            }
            // end updating blocks
            
            for(int j = itterator; j < itterator + LENGTH - 1; j++) {
                for (int k = 0; k < i.LENGTH; k++) {
                    probabilities[j + k] += 1 / (guarenteedLength + 1);
                }
            }
            itterator += i.LENGTH + 1;
        }

        for (int i = 0; i < 11; i ++) {
            if (probabilities[i] == 1.0) {
                positions[i] = 2;
            } else if (probabilities[i] > 0.0 && probabilities[i] < 1.0) {
                positions[i] = 1;
            } else if (probabilities[i] == 0.0) {
                positions[i] = 0;
            } else {
                positions[i] = -1;
            }
        }
    }
}

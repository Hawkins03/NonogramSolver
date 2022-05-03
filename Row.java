package NonogramSolver;
import NonogramSolver.*;
/**
 * a single row / column in a nongram "board"
 * @author Hawkins Peterson
 * @version 10.16.21
 */
public class Row {
    private final int LENGTH;
    private int[] blockLengths;
    private double[] probabilities; //psudo probabilities (0% = -1 and not 0
    private int blockSum;
    public Row(int LENGTH, int[] blockLengths) throws ArrayIndexOutOfBoundsException{

        //is this row valid (ie would the total length of the blocks be < the row length
        int sum = 0;
        for(int i : blockLengths)
            sum += i;
        int blockSum = sum + blockLengths.length - 1;

        if(blockSum > LENGTH) {
            throw new ArrayIndexOutOfBoundsException("Blocks total to " + blockSum + 
                    " but the length of the row is " + LENGTH);
        }

        //row is legit and can be instantiated
        this.LENGTH = LENGTH;
        this.blockSum = blockSum;
        probabilities = new double[LENGTH + 2]; //allowing for edges to occur

        //setting edges to be 0s (not possible for a barrier to occur)
        probabilities[0] = -1.0;
        probabilities[-1] = -1.0;
    }

    public void  formatProbabilities() {
        for (int i = 0; i < probabilities.length; i++) {
            if (probabilities[i] == 0.0)
                probabilities[i] = -1.0;
            else if (Math.abs(probabilities[i]) == 1.00)
                continue;
            else
                probabilities[i] = 0.0;
        }
    }

    public void probabilityAssesment() {
        int itterator = 0;
        for (int i : blockLengths) {
            for (int j = itterator; j < itterator + i + 1; j++) {
                boolean xFound = false;
                for (int k = 0; k < i; k++) {
                    if (itterator == 0 && probabilities[j + k] == 0.0)
                        itterator = j + k;
                    else if (itterator == 1) {
                        xFound = true;
                        break;
                    } else if (itterator != 0)
                        probabilities[j + k] += 1 / (blockSum + 1);
                }
                if (xFound)
                    break;
            }
            itterator += i + 1;
        }
    } // need to make sure this works

    public void findFinishedBlocks() {
        int minPos = 0;
        int maxPos;
        for (int i = 0; i < blockLengths.length; i++) {
            for(int j = 0; j < i; j++)
                minPos += probabilities[j];
            minPos++;
            maxPos = LENGTH + blockLengths[i] - minPos; //recheck math

            for(minPos = minPos; minPos < maxPos; minPos++) {
                switch (-1 * (int) (probabilities[minPos] - probabilities[minPos + 1])) { //-1 for ordering correctly
                    case -2: //1.0 followed by -1.0
                        try {
                            minPos -= blockLengths[i]; //move back the length of the block
                        } catch (Exception e) {
                            throw new ArrayIndexOutOfBoundsException("Error, found block clipping out of row");
                        }
                    case 2://-1.0 followed by 1.0
                        double[] arrayCopy = probabilities;
                        if (arrayCopy[minPos] != 1 && arrayCopy[maxPos] != 1) {
                            arrayCopy[minPos] = -1;
                            arrayCopy[maxPos] = -1;
                            for (int k = 0; k < arrayCopy.length; k++)
                                if (Math.abs(arrayCopy[k + minPos]) != -1)
                                    arrayCopy[k + minPos] = 1;

                        }

                }
            }
        }
    }
}

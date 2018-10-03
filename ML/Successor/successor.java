import java.util.*;

public class successor {
    public static class JugState {
        int[] Capacity = new int[]{0,0,0};
        int[] Content = new int[]{0,0,0};
        public JugState(JugState copyFrom)
        {
            this.Capacity[0] = copyFrom.Capacity[0];
            this.Capacity[1] = copyFrom.Capacity[1];
            this.Capacity[2] = copyFrom.Capacity[2];
            this.Content[0] = copyFrom.Content[0];
            this.Content[1] = copyFrom.Content[1];
            this.Content[2] = copyFrom.Content[2];
        }
        public JugState()
        {
        }
        public JugState(int A,int B, int C)
        {
            this.Capacity[0] = A;
            this.Capacity[1] = B;
            this.Capacity[2] = C;
        }
        public JugState(int A,int B, int C, int a, int b, int c)
        {
            this.Capacity[0] = A;
            this.Capacity[1] = B;
            this.Capacity[2] = C;
            this.Content[0] = a;
            this.Content[1] = b;
            this.Content[2] = c;
        }
 
        public void printContent()
        {
            System.out.println(this.Content[0] + " " + this.Content[1] + " " + this.Content[2]);
        }
        public ArrayList<JugState> getNextStates(){
            ArrayList<JugState> successors = new ArrayList<>();

            // TODO add all successors to the list
            for (int i = 0; i < 3; i++){
            	// filling the jug
            	JugState toAdd3 = new JugState(this);
            	toAdd3.Content[i] = toAdd3.Capacity[i];
            	if (!this.equals(toAdd3) && ! successors.contains(toAdd3)) successors.add(toAdd3);
            	
            	// pouring this jug's content to the ground
            	JugState toAdd = new JugState(this);
            	toAdd.Content[i] = 0;
            	if (!this.equals(toAdd) && !successors.contains(toAdd)) {
            		successors.add(toAdd);
	            	// tracing through the cases pouring to other jugs
            		// we do have a content which is not 0 to pour around, secured by the if condition above
	            	for (int j = 0; j < 3; j++) {
	            		// skip the case pouring itself
	            		if (i == j) continue;
	            		// skip the case when the other jug is already full
	            		if (this.Capacity[j] - this.Content[j] == 0) continue;
	            		JugState toAdd2 = new JugState(this);
	            		// when this's content is more than other's left capacity
	            		if (this.Capacity[j] - this.Content[j] <= this.Content[i]) {
	            			toAdd2.Content[i] -= this.Capacity[j] - this.Content[j];
	            			toAdd2.Content[j] = this.Capacity[j];
	            		}
	            		else {
	            			toAdd2.Content[i] = 0;
	            			toAdd2.Content[j] += this.Content[i];
	            		}
	            		// check whether to add
	            		if (!this.equals(toAdd2) && !successors.contains(toAdd2)){
	            			 successors.add(toAdd2);
	            		}
	            	}
            	}
            }
            return successors;
        }
        @Override
        public boolean equals(Object other) {
        	if (other instanceof JugState) {
        		JugState a = (JugState) other;
        		return Arrays.equals(this.Content, a.Content);
        	}
        	else return false;
        }
    }

    public static void main(String[] args) {
        if( args.length != 6 )
        {
            System.out.println("Usage: java successor [A] [B] [C] [a] [b] [c]");
            return;
        }

        // parse command line arguments
        JugState a = new JugState();
        a.Capacity[0] = Integer.parseInt(args[0]);
        a.Capacity[1] = Integer.parseInt(args[1]);
        a.Capacity[2] = Integer.parseInt(args[2]);
        a.Content[0] = Integer.parseInt(args[3]);
        a.Content[1] = Integer.parseInt(args[4]);
        a.Content[2] = Integer.parseInt(args[5]);

        // Implement this function
        ArrayList<JugState> asist = a.getNextStates();

        // Print out generated successors
        for(int i=0;i< asist.size(); i++)
        {
            asist.get(i).printContent();
        }

        return;
    }
}


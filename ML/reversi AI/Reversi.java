import java.util.*;

import com.sun.net.httpserver.Authenticator.Success;

class State {
    char[] board;

    public State(char[] arr) {
        this.board = Arrays.copyOf(arr, arr.length);
    }
/**
 * the method will return the game theory value of current node
 * 1 means 1 win
 * -1 means 2 win
 * @return game theory value of the state
 */
    public int getScore() {
    	int oneCount = 0;
    	int twoCount = 0;
    	for(int i = 0; i < board.length; i++) {
    		if(board[i] == '1') {
    			oneCount++;
    		}
    		if(board[i] == '2') {
    			twoCount++;
    		}
    	}
    	if (oneCount == twoCount) {
    		return 0;
    	}
    	else if (oneCount > twoCount) {
    		return 1;
    	}
    	else return -1;
    }
    
    /**
     * 
     * @return if current state is a terminal state
     */
    public boolean isTerminal() {
    	// a state is terminal iff both player cannot move, which include the case when board is filled
    	if (getSuccessors('1').length == 0 && getSuccessors('2').length == 0) {
    		return true;
    	}
    	else {
    		return false;
    	}
    }
    
    /**
     * the method returns an array of successor state for current state
     * the array is empty if no successor is possible
     * @param player the player who is going to play in current state
     * @return an array of successor states
     */
    public State[] getSuccessors(char player) {
    	List<State> result = new ArrayList<State>();
    	int[] directions = {-5,-4,-3,-1,1,3,4,5};
    	// exam every empty block
    	for (int i = 0; i < board.length; i++) {
    		if (board[i] == '0') {
    			// a list indicate which block need to be flip to palyer's color
    			List<Integer> toFlip = new ArrayList<Integer>();
    			// exam all 8 directions
    			for(int j = 0; j < directions.length; j++) {
    				exEmpty1Dir(i, directions[j], player, toFlip);
    			}
    			// if toFlip has content, then its an accessible move, flip the rest
    			if(toFlip.size() != 0) {
    				char[] temp = Arrays.copyOf(board, board.length);
    				temp[i] = player;
    				for(Integer a : toFlip) {
    					temp[a] = player;
    				}
    				// add the successor state generated to the list
    				result.add(new State(temp));	
    			}
    		}
    	}
    	State[] successors = new State[result.size()];
    	successors = result.toArray(successors);
        return successors;
    }
    /**
     * the method check one direction from the empty block, fill toFlip if possible
     * @param i index of empty block
     * @param a direction operator
     * @param player the player's color
     * @param toFlip the list indicate which block need to be flip to player's color
     */
    public void exEmpty1Dir(int i, int a, char player, List<Integer> toFlip) {
    	// if going up, lvl is 1, if going down, lvl is -1, if horizontal check, lvl is 0
    	int lvl = a/Math.abs(a);
    	if (a == 1 || a == -1) {
    		lvl = 0;
    	}
    	// index of the block where it is 3 blocks away from the empty block in the direction of a
    	int dididi = i + 3*a;
    	// index of the block where it is 2 blocks away from the empty block in the direction of a
    	int didi = i + 2*a;
    	// index of the block where it is 1 block away from the empty block in the direction of a
    	int di = i + a;
    	// if dididi's position is right
    	if (dididi >=0 && dididi <=15 && dididi/4 == i/4 + 3*lvl) {
    		// flip consecutive 2 block
    		if (board[dididi] == player && board[didi] != player && board[didi] != '0' && board[di] != player && board[di] != '0') {
    			toFlip.add(didi);
    			toFlip.add(di);
    		}
    		// flip only 1 block
    		else {
        		if(board[didi] == player && board[di] != '0' && board[di] != player) {
        			toFlip.add(di);
        		}
        	}
    	}
    	// if dididi is not valid, yet didi is valid
    	else if(didi >=0 && didi <=15 && didi/4 == i/4 + 2*lvl) {
    		// flip only 1 block
    		if(board[didi] == player && board[di] != '0' && board[di] != player) {
    			toFlip.add(di);
    		}
    	}
    }
    
    public void printState(int option, char player) {
    	if (option == 1) {
    		if (isTerminal()) {
    			return;
    		}
    		else {
	    		State[] successors = getSuccessors(player);
	    		if (successors.length != 0) {
		    		for (State a : successors) {
		    			System.out.println(a.getBoard());
		    		}
	    		}
	    		else {
	    			System.out.println(getBoard());
	    		}
    		}
    	}
    	else if(option == 2) {
    		if (!isTerminal()) {
    			System.out.println("non-terminal");
    		}
    		else {
    			System.out.println(getScore());
    		}
    	}
    	else if(option == 3) {
    		System.out.println(Minimax.run(this, player));
    		System.out.println(Minimax.opCount);
    		// this is important: reset opCount global variable after every time we make changes to it
    		Minimax.opCount = 0;
    	}
    	else if(option == 4) {
    		State[] successors = getSuccessors(player);
    		if (!isTerminal() && successors.length == 0) {
    			System.out.println(getBoard());
    		}
    		else if (isTerminal()) {
    			return;
    		}
    		else {
    			int opt;
    			if(player == '1') {
    				opt = Integer.MIN_VALUE;
    				// only changes when opt is strictly smaller
    				for (int i = 0; i < successors.length; i++) {
    					if(Minimax.run(successors[i], '2') > opt) {
    						opt = i;
    					}
    				}
    			}
    			else {
    				opt = Integer.MAX_VALUE;
    				// only changes when opt is strictly larger
    				for (int i = 0; i< successors.length; i++) {
    					if(Minimax.run(successors[i], '1') < opt) {
    						opt = i;
    					}
    				}
    			}
    			System.out.println(successors[opt].getBoard());
    		}
	    		
    	}
    	else if(option >= 5) {
    		if (option == 5) {
    			System.out.println(Minimax.run_with_pruning(this, player));
    			System.out.println(Minimax.opCount);
        		Minimax.opCount = 0;
    		}
    		// basically the same with flag = 400
    		if (option == 6) {
    			State[] successors = getSuccessors(player);
        		if (!isTerminal() && successors.length == 0) {
        			System.out.println(getBoard());
        		}
        		else if (isTerminal()) {
        			return;
        		}
        		else {
        			int opt;
        			if(player == '1') {
        				opt = Integer.MIN_VALUE;
        				for (int i = 0; i < successors.length; i++) {
        					if(Minimax.run_with_pruning(successors[i], '2') > opt) {
        						opt = i;
        					}
        				}
        			}
        			else {
        				opt = Integer.MAX_VALUE;
        				for (int i = 0; i< successors.length; i++) {
        					if(Minimax.run_with_pruning(successors[i], '1') < opt) {
        						opt = i;
        					}
        				}
        			}
        			System.out.println(successors[opt].getBoard());
        		}
    		}
    	}

    }

    public String getBoard() {
        StringBuilder builder = new StringBuilder();
        for (int i = 0; i < 16; i++) {
            builder.append(this.board[i]);
        }
        return builder.toString().trim();
    }
@Override
    public boolean equals(Object src) {
		if (src instanceof State) {
	        for (int i = 0; i < 16; i++) {
	            if (this.board[i] != ((State) src).board[i])
	                return false;
	        }
	        return true;
		}
		else return false;
    }
}

class Minimax {
	// operation count
	public static int opCount = 0;
	/**
	 * regular minimax
	 * @param curr_state current state
	 * @return best max does
	 */
	private static int max_value(State curr_state) {
		opCount++;
		// base case
		if (curr_state.isTerminal()) {
			return curr_state.getScore();
		}
		else {
			int i = Integer.MIN_VALUE;
			// pass to next player when is not terminal state but current player cannot move
			if (curr_state.getSuccessors('1').length == 0) {
				return min_value(curr_state);
			}
			else {
				for (State a : curr_state.getSuccessors('1')) {
					int min_a = min_value(a);
					if (min_a > i) i = min_a;
				}
				return i;
			}
		}
	}
	/**
	 * basically the same usage as max_value
	 * @param curr_state current state
	 * @return best min does
	 */
	private static int min_value(State curr_state) {
		
        // mirror of max_value
		opCount++;
		if (curr_state.isTerminal()) {
			return curr_state.getScore();
		}
		else {
			int i = Integer.MAX_VALUE;
			if (curr_state.getSuccessors('2').length == 0) {
				return max_value(curr_state);
			}
			else {
				for (State a : curr_state.getSuccessors('2')) {
					int max_a = max_value(a);
					if (max_a < i) i = max_a;
				}
				return i;
			}
		}
	}
	/**
	 * max_value but with pruning
	 * @param curr_state current state
	 * @param alpha best max does along terminal to this node
	 * @param beta best min does along terminal to this node
	 * @return best max does
	 */
	private static int max_value_with_pruning(State curr_state, int alpha, int beta) {
	    
        
		opCount++;
		if (curr_state.isTerminal()) {
			return curr_state.getScore();
		}
		else {
			if (curr_state.getSuccessors('1').length == 0) return min_value_with_pruning(curr_state, alpha, beta);
			for (State a : curr_state.getSuccessors('1')) {
				int min_a = min_value_with_pruning(a, alpha, beta);
				if (min_a > alpha) {
					alpha = min_a;
					if (alpha >= beta) {
						return beta;
					}
				}
			}
			return alpha;
		}
	}
	/**
	 * min_value but with pruning
	 * @param curr_state current state
	 * @param alpha best max does along terminal to this node
	 * @param beta best min does along terminal to this node
	 * @return best min does
	 */
	private static int min_value_with_pruning(State curr_state, int alpha, int beta) {
	    
        // basically same logical as above
		opCount++;
		if (curr_state.isTerminal()) {
			return curr_state.getScore();
		}
		else {
			if (curr_state.getSuccessors('2').length == 0) return max_value_with_pruning(curr_state, alpha, beta);
			for (State a: curr_state.getSuccessors('2')) {
				int max_a = max_value_with_pruning(a, alpha, beta);
				if (max_a < beta) {
					beta = max_a;
					if(alpha >= beta) {
						return alpha;
					}
				}
			}
			return beta;
		}
	}
	/**
	 * run minimax regularly
	 * @param curr_state current state
	 * @param player player about to play
	 * @return game theory value at root
	 */
	public static int run(State curr_state, char player) {

		if (player == '1') {	
			return max_value(curr_state);
		}
		else {
			return min_value(curr_state);
		}
	}
	/**
	 * same as run except with pruning
	 * @param curr_state current state
	 * @param player player about to play
	 * @return game theory value at root
	 */
	public static int run_with_pruning(State curr_state, char player) {
	    // basically same logic as above
		if (player == '1') {
			return max_value_with_pruning(curr_state, Integer.MIN_VALUE, Integer.MAX_VALUE);
		}
		else {
			return min_value_with_pruning(curr_state, Integer.MIN_VALUE, Integer.MAX_VALUE);
		}
	}
}

public class Reversi {
    public static void main(String args[]) {
        if (args.length != 3) {
            System.out.println("Invalid Number of Input Arguments");
            return;
        }
        int flag = Integer.valueOf(args[0]);
        char[] board = new char[16];
        for (int i = 0; i < 16; i++) {
            board[i] = args[2].charAt(i);
        }
        int option = flag / 100;
        char player = args[1].charAt(0);
        if ((player != '1' && player != '2') || args[1].length() != 1) {
            System.out.println("Invalid Player Input");
            return;
        }
        State init = new State(board);
        init.printState(option, player);
    }
}

import java.util.*;

class State {
	int[] board;
	State parentPt;
	int depth;

	public State(int[] arr) {
		this.board = Arrays.copyOf(arr, arr.length);
		this.parentPt = null;
		this.depth = 0;
	}

	public State[] getSuccessors() {
		State[] successors = new State[4];
		int[] successorsInt = new int[4];
		
		// Get the index of empty tile in the array
		int indexzero = 0;
		for (int i = 0; i < 9; i++) {
			if(board[i] == 0) {
				indexzero = i;
				break;
			}
		}
		
		// generate 4 possible successor states;
		// 1. empty tile moving down
		int[] boardDown = Arrays.copyOf(board, board.length);
		int targetIndexDown = indexzero  + 3;
		if (targetIndexDown > 8) targetIndexDown -= 9;
		elementSwap(boardDown, targetIndexDown, indexzero);
		successorsInt[0] = boardToint(boardDown);
		// 2. empty tile moving up
		int[] boardUp = Arrays.copyOf(board, board.length);
		int targetIndexUp = indexzero - 3;
		if (targetIndexUp < 0) targetIndexUp += 9;
		elementSwap(boardUp, targetIndexUp, indexzero);
		successorsInt[1] = boardToint(boardUp);
		// 3. empty tile moving left
		int[] boardLeft = Arrays.copyOf(board, board.length);
		int currentRow = indexzero/3;
		int afterLshift = indexzero - 1;
		int targetIndexLeft = 0;
		if (afterLshift < 0) targetIndexLeft = 2;
		else {
			if (afterLshift/3 == currentRow) targetIndexLeft = afterLshift;
			else targetIndexLeft = indexzero + 2;
		}
		elementSwap(boardLeft, targetIndexLeft, indexzero);
		successorsInt[2] = boardToint(boardLeft);
		// 4. empty tile moving right
		int[] boardRight = Arrays.copyOf(board, board.length);
		int afterRshift = indexzero + 1;
		int targetIndexRight = 0;

		if (afterRshift/3 == currentRow) targetIndexRight = afterRshift;
		else targetIndexRight = indexzero - 2;
		
		elementSwap(boardRight, targetIndexRight, indexzero);
		successorsInt[3] = boardToint(boardRight);
		
		// sort
		Arrays.sort(successorsInt);
		
		for (int i = 0; i < successorsInt.length; i++) {
			successors[i] = new State(intToboard(successorsInt[i]));
			successors[i].depth = this.depth + 1;
			successors[i].parentPt = this;
		}
		return successors;
	}

	public void printState(int option) {
		
		if (option == 1 || option == 2) {
			System.out.println(getBoard());
		}
		if (option == 3) {
			if (this.parentPt == null)
				System.out.println(getBoard() + " parent 0 0 0 0 0 0 0 0 0");
			else
				System.out.println(getBoard() + " parent " + parentPt.getBoard());
		}
	}

	public String getBoard() {
		StringBuilder builder = new StringBuilder();
		for (int i = 0; i < 9; i++) {
			builder.append(this.board[i]).append(" ");
		}
		return builder.toString().trim();
	}

	public boolean isGoalState() {
		for (int i = 0; i < 9; i++) {
			if (this.board[i] != (i + 1) % 9)
				return false;
		}
		return true;
	}
	
	//override to use the build in method
	@Override
	public boolean equals(Object src) {
		if (src instanceof State) {
			for (int i = 0; i < 9; i++) {
				if (this.board[i] != ((State) src).board[i])
					return false;
			}
		}
		return true;
	}
	
	public void elementSwap(int[] a, int b, int c) {
		int temp = a[b];
		a[b] = a[c];
		a[c] = temp;
		return;
	}
	
	public static int boardToint(int[]a) {

//		StringBuilder strNum = new StringBuilder();
//		for (int e : a) {
//			strNum.append(e);
//		}
//		int finalInt = 	Integer.parseInt(strNum.toString());
//		return finalInt;
		int result = 0;
		for (int i = 8; i>=0; i--) {
			result += a[8-i] * Math.pow(10, i);
		}
		return result;
	}
	
	public static int[] intToboard(int a) {
		String str = Integer.toString(a);
		// left justified paddle
		if (str.length() < 9) str = "0" + str;
		int[] result = new int[9];
		for (int i = 0; i < 9; i++) {
			result[i] = Integer.parseInt(Character.toString(str.charAt(i)));
		}
		return result;
	}
}

public class Torus {

	public static void main(String args[]) {
		if (args.length < 10) {
			System.out.println("Invalid Input");
			return;
		}
		int flag = Integer.valueOf(args[0]);
		int[] board = new int[9];
		for (int i = 0; i < 9; i++) {
			board[i] = Integer.valueOf(args[i + 1]);
		}
		int option = flag / 100;
		int cutoff = flag % 100;
		if (option == 1) {
			State init = new State(board);
			State[] successors = init.getSuccessors();
			for (State successor : successors) {
				successor.printState(option);
			}
		} 
		else {
			State init = new State(board);
			Stack<State> stack = new Stack<>();
			List<State> prefix = new ArrayList<>();
			int goalChecked = 0;
			int maxStackSize = Integer.MIN_VALUE;

			// needed for Part E
			while (true) {
				// clear prefix after an iteration of DFS
				prefix.clear();
				stack.push(init);
				// update Max every time we push
				if (stack.size() > maxStackSize) {
					maxStackSize = stack.size();
				}
				while (!stack.isEmpty()) {
					State current = stack.pop();
					goalChecked += 1;
					if(current.isGoalState()) {
						for (State s : prefix) {
							s.printState(1);
						}
						current.printState(1);
						System.out.println("Goal-Check " + goalChecked);
						System.out.println("Max-stack-size " + maxStackSize);
						return;
					}
					// out to console on-the-go
					if(option == 2 || option == 3) current.printState(option);
					// if current is init
					if (current.parentPt == null) prefix.add(current);
					// modifying prefix
					else {
						prefix = new ArrayList<State>(prefix.subList(0, prefix.indexOf(current.parentPt) + 1));
						prefix.add(current);
					}
					// branch for not option 4
					if (option != 4) {
						// skip generating sub nodes if cutoff reached
						if (current.depth >= cutoff) continue;
						// else go deeper
						State[] Csuccessors = current.getSuccessors();
//						int stacksizenow = stack.size();
						for (int i = 0; i < Csuccessors.length; i++) {
							if (!prefix.contains(Csuccessors[i])) {
								stack.push(Csuccessors[i]);
							}
						}
						if (stack.size() > maxStackSize) {
							maxStackSize=stack.size();
//							System.out.println("maxsizenow " + maxStackSize + " cutoff " + cutoff);
//							System.out.println("size before inserting: " + stacksizenow);
//							System.out.println("Csuccessornow: ");
//							for (State s:Csuccessors) {
//								s.printState(1);
//							}
//							System.out.println("Prefixnow: ");
//							for (State s: prefix) {
//								s.printState(1);
//							}
//							System.out.println("-------------------");
						}
					}
					// for option 4
					else {
						// print the path
						if (current.depth >= cutoff) {
							for (State s : prefix) {
								s.printState(1);
							}
							break;
						}
						// go deeper if cutoff not reached
						else {
							State[] Csuccessors = current.getSuccessors();
							for (int i = 0; i < Csuccessors.length; i++) {
								if (!prefix.contains(Csuccessors[i])) {
									stack.push(Csuccessors[i]);
								}
							}
						}
					}
				}
				if (option != 5)
					break;
				// enter next DFS iteration with larger cutoff
				cutoff++;
			}
		}
	}
}

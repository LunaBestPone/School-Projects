import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

public class Neural {
	static ArrayList<double[]> trSet = new ArrayList<double[]>();
	static ArrayList<double[]> evSet = new ArrayList<double[]>();
	static ArrayList<double[]> tsSet = new ArrayList<double[]>();
	
	private static double[] updateWSet(double[] wSet, double[] dwSet, double n) {
		double result[] = new double[9];
		for (int i = 0; i < 9; i++) {
			result[i] = sto(wSet[i], dwSet[i], n);
		}
		return result;
	}
	private static double vC(double[] wSet, double x1, double x2) {
		double uA = u(wSet[0], wSet[1], x1, wSet[2], x2);
		double vA = ReLU(uA);
		
		double uB = u(wSet[3], wSet[4], x1, wSet[5], x2);
		double vB = ReLU(uB);
		
		double uC = u(wSet[6], wSet[7], vA, wSet[8], vB);
		double vC = sigmoid(uC);
		return vC;
	}
	
	private static double u(double w1, double w2, double v1, double w3, double v2) {
		return (w1 + w2*v1 + w3*v2);
	}
	
	private static double ReLU(double u) {
		return Double.max(u, 0l);
	}
	
	private static double sigmoid(double u) {
		return (1 / (1 + Math.exp(-u)));
	}
	
	private static double E(double vC, double y) {
		return Math.pow(vC - y, 2)/2;
	}
	
	private static double dvC(double vC, double y) {
		return vC - y;
	}
	
	private static double duC(double vC, double y) {	
		return dvC(vC, y) * vC * (1 - vC);
	}
	
	private static double dvJ(double w, double duC) {
		return w * duC;
	}
	
	private static double duJ(double dvJ, double uJ) {
		if (uJ >= 0) {
			return dvJ;
		}
		else return 0;
	}
	
	private static double dwIJ(double vi, double duJ) {
		return vi * duJ;
	}
	
	private static double[] dw(double[] wSet, double x1, double x2, double y) {
		double uA = u(wSet[0], wSet[1], x1, wSet[2], x2);
		double vA = ReLU(uA);
		double uB = u(wSet[3], wSet[4], x1, wSet[5], x2);
		double vB = ReLU(uB);
		double vC = vC(wSet, x1, x2);
		double duC = duC(vC, y);
		
		double dvA = dvJ(wSet[7], duC);		
		double dvB = dvJ(wSet[8], duC);
		
		double dw1 = dwIJ(1, duJ(dvA, uA));
		double dw2 = dwIJ(x1, duJ(dvA, uA));
		double dw3 = dwIJ(x2, duJ(dvA, uA));
		double dw4 = dwIJ(1, duJ(dvB, uB));
		double dw5 = dwIJ(x1, duJ(dvB, uB));
		double dw6 = dwIJ(x2, duJ(dvB, uB));
		double dw7 = dwIJ(1, duC(vC, y));
		double dw8 = dwIJ(vA, duC(vC, y));
		double dw9 = dwIJ(vB, duC(vC, y));
		
		double result[] = {dw1, dw2, dw3, dw4, dw5, dw6, dw7, dw8, dw9};
		return result;
		
	}
	
	private static double sto(double w, double dw, double n) {
		return w - n * dw;
	}
	
	private static double setE(double[] wSet) {
		double result = 0l;
		for(int i = 0; i < 25; i++) {
			result += Math.pow(vC(wSet, evSet.get(i)[0], evSet.get(i)[1]) - evSet.get(i)[2], 2)/2;
		}
		return result;
	}
	
	private static double[] epoch(int flag, double[] wSet, double n) {
		for(int i = 0; i < 67; i++) {
			double x1 = trSet.get(i)[0];
			double x2 = trSet.get(i)[1];
			double y = trSet.get(i)[2];
			if (flag == 600) System.out.printf("%.5f %.5f %.5f\n", x1, x2, y);
			double[] dwSet = dw(wSet, x1, x2, y);
			wSet = updateWSet(wSet, dwSet, n);	
			if (flag == 600) {
				for(int x = 0; x < 8; x++) {
					System.out.printf("%.5f ", wSet[x]);
				}
				System.out.printf("%.5f\n", wSet[8]);
				double setE = setE(wSet);
				System.out.printf("%.5f\n", setE);
			}
		}
		if (flag == 700) {
			for(int x = 0; x < 8; x++) {
				System.out.printf("%.5f ", wSet[x]);
			}
			System.out.printf("%.5f\n", wSet[8]);
			System.out.printf("%.5f\n", setE(wSet));
		}
		return wSet;
	}
	
	public static void main(String[] args) {
		int flag = Integer.parseInt(args[0]);
		
		try {
			File tr = new File("hw2_midterm_A_train.txt");
			File ev = new File("hw2_midterm_A_eval.txt");
			File ts = new File("hw2_midterm_A_test.txt");
			Scanner trin = new Scanner(tr);
			Scanner evin = new Scanner(ev);
			Scanner tsin = new Scanner(ts);
			
			while(trin.hasNextLine()) {
				double[] temp = new double[3];
				String temp2 = trin.nextLine();
				String[] splited = temp2.split(" ");
				for(int i = 0; i < 3; i++) {
					temp[i] = Double.parseDouble(splited[i]);
				}
				trSet.add(temp);
			}
			while(evin.hasNextLine()) {
				double[] temp = new double[3];
				String temp2 = evin.nextLine();
				String[] splited = temp2.split(" ");
				for(int i = 0; i < 3; i++) {
					temp[i] = Double.parseDouble(splited[i]);
				}
				evSet.add(temp);
			}
			while(tsin.hasNextLine()) {
				double[] temp = new double[3];
				String temp2 = tsin.nextLine();
				String[] splited = temp2.split(" ");
				for(int i = 0; i < 3; i++) {
					temp[i] = Double.parseDouble(splited[i]);
				}
				tsSet.add(temp);
			}
		}
		catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		
		double wSet[] = new double[9];
		for (int i = 0; i < 9; i++) {
			wSet[i] = Double.parseDouble(args[i+1]);
		}
		
		if (flag == 100) {
			double x1 = Double.parseDouble(args[10]);
			double x2 = Double.parseDouble(args[11]);
			
			double uA = u(wSet[0], wSet[1], x1, wSet[2], x2);
			double vA = ReLU(uA);
			
			double uB = u(wSet[3], wSet[4], x1, wSet[5], x2);
			double vB = ReLU(uB);
			
			double uC = u(wSet[6], wSet[7], vA, wSet[8], vB);
			double vC = sigmoid(uC);
			
			System.out.printf("%.5f %.5f %.5f %.5f %.5f %.5f\n", uA, vA, uB, vB, uC, vC);
		}

		if (flag == 200) {
			double x1 = Double.parseDouble(args[10]);
			double x2 = Double.parseDouble(args[11]);
			double y = Double.parseDouble(args[12]);

			double vC = vC(wSet, x1, x2);
			
			double E = E(vC, y);
			double dvC = dvC(vC, y);
			double duC = duC(vC, y);
			
			System.out.printf("%.5f %.5f %.5f\n", E, dvC, duC);
		}
		
		if (flag == 300) {
			double x1 = Double.parseDouble(args[10]);
			double x2 = Double.parseDouble(args[11]);
			double y = Double.parseDouble(args[12]);
			
			double uA = u(wSet[0], wSet[1], x1, wSet[2], x2);
			double uB = u(wSet[3], wSet[4], x1, wSet[5], x2);
			double vC = vC(wSet, x1, x2);
			double duC = duC(vC, y);
			
			double dvA = dvJ(wSet[7], duC);
			double duA = duJ(dvA, uA);
			
			double dvB = dvJ(wSet[8], duC);
			double duB = duJ(dvB, uB);
			
			System.out.printf("%.5f %.5f %.5f %.5f\n", dvA, duA, dvB, duB);
		}
		
		if (flag == 400) {
			double x1 = Double.parseDouble(args[10]);
			double x2 = Double.parseDouble(args[11]);
			double y = Double.parseDouble(args[12]);
			
			double dwSet[] = dw(wSet, x1, x2, y);
			
			System.out.printf("%.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\n", dwSet[0], dwSet[1], dwSet[2], dwSet[3], dwSet[4], dwSet[5], dwSet[6], dwSet[7], dwSet[8]);
		}
		
		if (flag == 500) {
			double x1 = Double.parseDouble(args[10]);
			double x2 = Double.parseDouble(args[11]);
			double y = Double.parseDouble(args[12]);
			double n = Double.parseDouble(args[13]);
			
			for(int i = 0; i < 8; i++) {
				System.out.printf("%.5f ", wSet[i]);
			}
			System.out.printf("%.5f\n", wSet[8]);
			
			double E = E(vC(wSet, x1, x2), y);
			System.out.printf("%.5f\n", E);
			
			double dwSet[] = dw(wSet, x1, x2, y);
			wSet = updateWSet(wSet, dwSet, n);

			for(int i = 0; i < 8; i++) {
				System.out.printf("%.5f ", wSet[i]);
			}
			System.out.printf("%.5f\n", wSet[8]);
			
			E = E(vC(wSet, x1, x2), y);
			System.out.printf("%.5f\n", E);
		}
		
		if (flag == 600) {
			double n = Double.parseDouble(args[10]);
			wSet = epoch(flag, wSet, n);
		}
		
		if (flag == 700) {
			double n = Double.parseDouble(args[10]);
			double t = Double.parseDouble(args[11]);
			
			for (int i = 0; i < t; i++) {
				wSet = epoch(flag, wSet, n);
			}
		}
		
		if (flag == 800) {
			double n = Double.parseDouble(args[10]);
			double t = Double.parseDouble(args[11]);
			int counter = 1;
			int hit = 0;
			wSet = epoch(flag, wSet, n);
			double oldSetE = Double.MAX_VALUE;
			double newSetE = setE(wSet);
			while (newSetE <= oldSetE && counter < t) {
				wSet = epoch(flag, wSet, n);
				oldSetE = newSetE;
				newSetE = setE(wSet);
				counter++;
			}
			for (int i = 0; i < 25; i++) {
				double vC = vC(wSet, tsSet.get(i)[0], tsSet.get(i)[1]);
				if (vC >= 0.5) {
					if (((int) tsSet.get(i)[2]) == 1) hit++;
				}
				else {
					if (((int)tsSet.get(i)[2]) == 0) hit++;
				}
			}
			System.out.println(counter);
			for(int x = 0; x < 8; x++) {
				System.out.printf("%.5f ", wSet[x]);
			}
			System.out.printf("%.5f\n", wSet[8]);
			System.out.printf("%.5f\n", newSetE);
			System.out.printf("%.5f\n", hit / 25.0);
		}
	}

}

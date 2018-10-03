import java.util.Random;

public class Ice {

	public static void main(String[] args) {
		int flag = Integer.parseInt(args[0]);
	
		//initializing years
		Double years[] = new Double[162];
		for (int i = 0; i < 162; i++) {
			years[i] = (double) (i + 1855);
		}
		
		//initializing days
		Double days[] = {118D, 151D, 121D, 96D, 110D, 117D, 132D, 104D, 125D, 118D, 125D, 123D, 110D, 127D, 131D, 99D, 126D, 144D, 136D, 126D, 91D, 130D, 62D, 112D, 99D, 161D, 78D, 124D, 119D, 124D, 128D, 131D, 113D, 88D, 75D, 111D, 97D, 112D, 101D, 101D, 91D, 110D, 100D, 130D, 111D, 107D, 105D, 89D, 126D, 108D, 97D, 94D, 83D, 106D, 98D, 101D, 108D, 99D, 88D, 115D, 102D, 116D, 115D, 82D, 110D, 81D, 96D, 125D, 104D, 105D, 124D, 103D, 106D, 96D, 107D, 98D, 65D, 115D, 91D, 94D, 101D, 121D, 105D, 97D, 105D, 96D, 82D, 116D, 114D, 92D, 98D, 101D, 104D, 96D, 109D, 122D, 114D, 81D, 85D, 92D, 114D, 111D, 95D, 126D, 105D, 108D, 117D, 112D, 113D, 120D, 65D, 98D, 91D, 108D, 113D, 110D, 105D, 97D, 105D, 107D, 88D, 115D, 123D, 118D, 99D, 93D, 96D, 54D, 111D, 85D, 107D, 89D, 87D, 97D, 93D, 88D, 99D, 108D, 94D, 74D, 119D, 102D, 47D, 82D, 53D, 115D, 21D, 89D, 80D, 101D, 95D, 66D, 106D, 97D, 87D, 109D, 57D, 87D, 117D, 91D, 62D, 65D};

		// printing out the data set
		if (flag == 100) {
			for(int i = 0; i < years.length; i++) {
				System.out.printf("%d %d\n", years[i], days[i].intValue());
			}
		}
		
		// printing the size of the data set, sample mean and standard deviation
		else if(flag == 200) {
			System.out.println(years.length);
			
			double dSum = 0D;
			double sMean = 0D;
			for (int i = 0; i < 162; i++) {
				dSum += days[i];
			}
			sMean = dSum/ ((double) 162);
			System.out.printf("%.2f\n", sMean);
			
			double devSum = 0D;
			double dev = 0D;
			for (int i = 0; i< 162; i++) {
				devSum += Math.pow(days[i] - sMean, 2);
			}
			dev = devSum/((double) (161));
			System.out.printf("%.2f\n", dev);
		}
		
		// printing MSE
		else if(flag == 300) {
			double beta0 = Double.parseDouble(args[1]);
			double beta1 = Double.parseDouble(args[2]);
			double MSE = 0D;
			MSE = MSE(beta0, beta1, years, days);
			
			System.out.printf("%.2f\n", MSE);
		}
		
		// gradient descent
		else if(flag == 400) {
			double beta0 = Double.parseDouble(args[1]);
			double beta1 = Double.parseDouble(args[2]);
			double g1 = 0D;
			double g2 = 0D;
			double[] result = G(beta0, beta1, years, days);
			g1 = result[0];
			g2 = result[1];
			System.out.printf("%.2f\n%.2f\n", g1, g2);
		}
		
		// finding the T time gradient
		else if(flag == 500) {
			double n = Double.parseDouble(args[1]);
			int T = Integer.parseInt(args[2]);
			
			iterativeGD(n, T, years, days, 0);
		}
		
		// closed form solution
		else if(flag == 600) {
			double[] result = Gcap(years, days);
			double beta1cap = result[0];
			double beta0cap = result[1];
			double MSE = MSE(beta0cap, beta1cap, years, days);
			System.out.printf("%.2f %.2f %.2f\n", beta0cap, beta1cap, MSE);
			
		}
		
		// predicting with betacaps
		else if (flag == 700) {
			Double pYear = Double.parseDouble(args[1]);
			double[] temp = Gcap(years, days);
			double beta1cap = temp[0];
			double beta0cap = temp[1];
			double result = beta0cap + beta1cap * pYear;
			System.out.printf("%.2f\n", result);
		}
		
		// gradient descent but with normalized scale for x
		else if (flag == 800) {
			Double[] stdYears = NX();

			double n = Double.parseDouble(args[1]);
			int T = Integer.parseInt(args[2]);
			
			iterativeGD(n, T, stdYears, days, 0);
		}
		
		// SGD
		else if (flag == 900) {
			Double[] stdYears = NX();
			
			double n = Double.parseDouble(args[1]);
			int T = Integer.parseInt(args[2]);
			
			iterativeGD(n, T, stdYears, days, 1);
		}
	}
	/**
	 * calculate gradient
	 * @param beta0 beta0
	 * @param beta1 beta1
	 * @param years the x
	 * @param days the y
	 * @return the gradient on MSE
	 */
	private static double[] G(double beta0, double beta1, Double[] years, Double[] days) {
		double g1Sum = 0D;
		double g2Sum = 0D;
		double g1 = 0D;
		double g2 = 0D;

		for(int i = 0; i < 162; i++) {
			double temp = beta0 + beta1 * years[i] - days[i];
			g1Sum += temp;
			g2Sum += temp * years[i];
		}
		
		g1 = 2 * g1Sum/((double) 162);
		g2 = 2 * g2Sum/((double) 162);
		double[] result = {g1, g2};
		return result;
	}
	
	/**
	 * a SGD version of G function
	 * @param beta0 beta0
	 * @param beta1 beta1
	 * @param years the x
	 * @param days the y
	 * @return the gradient on MSE
	 */
	private static double[] SGDG(double beta0, double beta1, Double[] years, Double[] days) {
		Random rand = new Random();
		int rNum = rand.nextInt(162);
		double g1 = 2 * (beta0 + beta1 * years[rNum] - days[rNum]);
		double g2 = 2 * (beta0 + beta1 * years[rNum] - days[rNum]) * years[rNum];
		double[] result = {g1, g2};
		return result;
	}
	
	/**
	 * calculate MSE
	 * @param beta0 beta0
	 * @param beta1 beta1
	 * @param years the x
	 * @param days the y
	 * @return the MSE
	 */
	private static double MSE(double beta0, double beta1, Double[] years, Double[] days) {
		double Msum = 0D;
		double MSE = 0D;
		for(int i = 0; i < 162; i++) {
			Msum += Math.pow(beta0 + beta1 * years[i] - days[i], 2);
		}
		MSE = Msum/((double) 162);
		return MSE;
	}
	
	/**
	 * a closed form solution for beta
	 * @param years the x
	 * @param days the y
	 * @return the betas stored in an array
	 */
	private static double[] Gcap(Double years[], Double days[]) {
		double avgYr = 1935.5;
		double avgDys = 0D;
		for (int i = 0; i < 162; i++) {
			avgDys += days[i];
		}
		avgDys /= ((double) 162);
		double beta1capTemp1 = 0D;
		double beta1capTemp2 = 0D;
		for (int i = 0; i < 162; i++) {
			beta1capTemp1 += (years[i] - avgYr) * (days[i] - avgDys);
			beta1capTemp2 += Math.pow((years[i] - avgYr), 2);
		}
		double beta1cap = beta1capTemp1/beta1capTemp2;
		double beta2cap = avgDys - beta1cap * avgYr;
		double result[] = {beta1cap, beta2cap};
		return result;
	}
	
	/**
	 * iterative gradient descent
	 * @param n step size
	 * @param T number of iterations
	 * @param years the x
	 * @param days the y
	 * @param SGDOPT 0-regular, 1-SGD approach
	 */
	private static void iterativeGD(double n, int T, Double[] years, Double[] days, int SGDOPT) {
		double g1 = 0D;
		double g2 = 0D;
		double MSE = 0D;
		double[] temp;
		for (int i = 1; i <= T; i++) {
			if(SGDOPT == 0) {
				temp = G(g1, g2, years, days);
			}
			else {
				temp = SGDG(g1, g2, years, days);
			}
			g1 = g1 - n * temp[0];
			g2 = g2 - n * temp[1];
			MSE = MSE(g1, g2, years, days);
			System.out.printf("%d %.2f %.2f %.2f\n", i, g1, g2, MSE);
		}
	}
	
	/**
	 * generate the normalized x
	 * @return the normalized x list
	 */
	private static Double[] NX() {
		double avgYr = 1935.5;
		double stdx = 0D;
		for (int i = 1855; i <= 2016; i++) {
			stdx += Math.pow(i - avgYr, 2);
		}
		stdx /= 161;
		stdx = Math.sqrt(stdx);
		Double[] stdYears = new Double[162];
		for(int i = 0; i < 162; i++) {
			stdYears[i] = (i + 1855 - avgYr)/stdx;
		}
		return stdYears;
	}
}

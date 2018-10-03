
import java.util.*;
import java.io.*;

public class Chatbot{
	//cache to avoid extra calculation, maybe broke some machine of insufficient memory
	private static TreeMap<Integer, Integer[]> AFTER_H = new TreeMap<Integer, Integer[]>();
	private static Hashtable<Integer[], Integer[]> AFTER_H1_H2 = new Hashtable<Integer[], Integer[]>();
    private static String filename = "./WARC201709_wid.txt";
    private static ArrayList<Integer> readCorpus(){
        ArrayList<Integer> corpus = new ArrayList<Integer>();
        try{
            File f = new File(filename);
            Scanner sc = new Scanner(f);
            while(sc.hasNext()){
                if(sc.hasNextInt()){
                    int i = sc.nextInt();
                    corpus.add(i);
                }
                else{
                    sc.next();
                }
            }
        }
        catch(FileNotFoundException ex){
            System.out.println("File Not Found.");
        }
        return corpus;
    }
    /**
     * get unigram probability
     * @param corpus
     * @param index
     * @return unigram probability for index
     */
    static private double getUniProb(ArrayList<Integer> corpus, int index) {
    	int count = 0;
    	count = Collections.frequency(corpus, index);
    	double result = count/(double)corpus.size();
    	return result;
    }
    /**
     * get bigram probability
     * @param corpus
     * @param h the word as condition
     * @param index
     * @return bigram probability for index, h as condition
     */
    static private double getBiProb(ArrayList<Integer> corpus, int h, int index) {
    	int word_after_h = 0;
    	int pair = 0;
    	// if the word after h and pair are already cached in static container, just take it
    	if (AFTER_H.containsKey(h) && AFTER_H.get(h)[index + 1] != null) {
    		pair = AFTER_H.get(h)[index + 1];
    		word_after_h = AFTER_H.get(h)[0];
    	}
    	// else, do corresponding calculations
    	else if (AFTER_H.containsKey(h)){
    		for (int i = 0; i < corpus.size() - 1; i++) {
	    		if (corpus.get(i) == h) {
	    			if (corpus.get(i+1) == index) {
	    				pair++;
	    			}
	    		}
	    	}
	    	AFTER_H.get(h)[index + 1] = pair;
	    	word_after_h = AFTER_H.get(h)[0];
    	}
    	// container serve as cache is empty
    	else {
	    	for (int i = 0; i < corpus.size() - 1; i++) {
	    		if (corpus.get(i) == h) {
	    			word_after_h++;
	    			if (corpus.get(i+1) == index) {
	    				pair++;
	    			}
	    		}
	    	}
	    	Integer[] temp = new Integer[4701];
	    	AFTER_H.put(h, temp);
	    	AFTER_H.get(h)[0] = word_after_h;
	    	AFTER_H.get(h)[index + 1] = pair;
    	}
    	return ((double) pair)/ word_after_h;
    }
    /**
     * get trigram probability
     * @param corpus
     * @param h1 conditional word 1
     * @param h2 conditional word 2
     * @param index
     * @return the trigram probability of index, h1, h2 as condition
     */
    static private double getTriProb(ArrayList<Integer> corpus,int h1, int h2, int index) {
    	int word_after = 0;
    	int pair = 0;
    	Integer[] tempPre = {h1, h2};
    	// same logic for utilizing cache
    	if (AFTER_H1_H2.containsKey(tempPre) && AFTER_H1_H2.get(tempPre)[index + 1] != null) {
    		pair = AFTER_H1_H2.get(tempPre)[index + 1];
    		word_after = AFTER_H1_H2.get(tempPre)[0];
    	}
    	else if (AFTER_H1_H2.containsKey(tempPre)){
    		for (int i = 0; i < corpus.size() - 2; i++) {
	    		if (corpus.get(i) == h1 && corpus.get(i+1) == h2) {
	    			if (corpus.get(i+2) == index) {
	    				pair++;
	    			}
	    		}
	    	}
	    	AFTER_H1_H2.get(tempPre)[index + 1] = pair;
	    	word_after = AFTER_H1_H2.get(tempPre)[0];
    	}
    	else {
	    	for (int i = 0; i < corpus.size() - 2; i++) {
	    		if (corpus.get(i) == h1 && corpus.get(i+1) == h2) {
	    			word_after++;
	    			if (corpus.get(i+2) == index) {
	    				pair++;
	    			}
	    		}
	    	}
	    	Integer[] temp = new Integer[4701];
	    	AFTER_H1_H2.put(tempPre, temp);
	    	AFTER_H1_H2.get(tempPre)[0] = word_after;
	    	AFTER_H1_H2.get(tempPre)[index + 1] = pair;
    	}
    	
    	if (word_after == 0) return 0;
    	return ((double) pair)/ word_after;
    }
    /**
     * generate the segments
     * @param corpus
     * @param opt 1 for uni, 2 for bi, 3 + for tri
     * @param h1 conditional word 1, 0 if not used
     * @param h2 conditional word 2, 0 if not used
     * @return the generated segments
     */
    static private TreeMap<Integer, Double[]> getSegz(ArrayList<Integer> corpus, int opt, int h1, int h2) {
    	Double left = (double) 0;
    	Double right;
    	if (opt == 1) {
    		right = getUniProb(corpus, 0);
    	}
    	else if (opt == 2) {
    		right = getBiProb(corpus, h1, 0);
    	}
    	else {
            right = getTriProb(corpus, h1, h2, 0);
    	}
        TreeMap<Integer, Double[]> treemap = new TreeMap<Integer, Double[]>();
        int validEntryCounter = 0;
        if (!left.equals(right)) {
        	Double[] pair = {((double) 0), left, right};
        	treemap.put(0, pair);
        	validEntryCounter++;
        }
        for (int i = 1; i < 4700; i++) {
        	left = right;
        	if (opt == 1) {
        		right += getUniProb(corpus, i);
        	}
        	else if (opt == 2) {
        		right += getBiProb(corpus, h1, i);	
        	}
        	else {
                right += getTriProb(corpus, h1, h2, i);
        	}
        	if (!left.equals(right)) {
            	Double[] pairv2 = {((double) i),left, right};
            	treemap.put(validEntryCounter, pairv2);
            	validEntryCounter++;
        	}
        }
        // clear the cache, or the machine is gonna explode
        AFTER_H.clear();
        AFTER_H1_H2.clear();
        return treemap;
    }
    
    /**
     * regular output function, in charge of console output and choose word based on r
     * @param treemap the segment
     * @param r the random number
     * @param opt 1 enables console output
     * @return the chosen word
     */
    static private int regOutput(TreeMap<Integer, Double[]> treemap, double r, int opt) {
    	if (treemap.get(treemap.firstKey())[1] <= r && r <= treemap.get(treemap.firstKey())[2]) {
    		if (opt == 1) {
	        	System.out.println(treemap.get(treemap.firstKey())[0].intValue());
	        	System.out.println(String.format("%.7f", treemap.get(treemap.firstKey())[1]));
	        	System.out.println(String.format("%.7f", treemap.get(treemap.firstKey())[2]));
    		}
        	return treemap.get(treemap.firstKey())[0].intValue();
        }
        else {
            for (int i = 1; i < treemap.size(); i++) {
            	if (treemap.get(i)[1] < r && r <= treemap.get(i)[2]) {
            		if (opt == 1) {
	            		System.out.println(treemap.get(i)[0].intValue());
	            		System.out.println(String.format("%.7f", treemap.get(i)[1]));
	                	System.out.println(String.format("%.7f", treemap.get(i)[2]));
            		}
                	return treemap.get(i)[0].intValue();
            	}
            }
        }
    	return -1;
    }

    static public void main(String[] args){
        ArrayList<Integer> corpus = readCorpus();
		int flag = Integer.valueOf(args[0]);
        
        if(flag == 100){
			int w = Integer.valueOf(args[1]);
            int count = 0;
            count = Collections.frequency(corpus, w);
            System.out.println(count);
            System.out.println(String.format("%.7f",count/(double)corpus.size()));
        }
        else if(flag == 200){
            int n1 = Integer.valueOf(args[1]);
            int n2 = Integer.valueOf(args[2]);
            // the random number
            double r = ((double) n1) / n2;
            // the segments
            TreeMap<Integer, Double[]> treemap = getSegz(corpus, 1, 0, 0);
            // select a segment
            regOutput(treemap, r, 1);
        }

        else if(flag == 300){
            int h = Integer.valueOf(args[1]);
            int w = Integer.valueOf(args[2]);
            int count = 0;
            ArrayList<Integer> words_after_h = new ArrayList<Integer>();
            
            for(int i = 0; i < corpus.size() - 1; i++) {
            	if (corpus.get(i) == h) {
            		words_after_h.add(i + 1);
            		if (corpus.get(i + 1) == w) {
            			count++;
            		}
            	}
            }
            //output 
            System.out.println(count);
            System.out.println(words_after_h.size());
            System.out.println(String.format("%.7f",count/(double)words_after_h.size()));
        }
        else if(flag == 400){
            int n1 = Integer.valueOf(args[1]);
            int n2 = Integer.valueOf(args[2]);
            int h = Integer.valueOf(args[3]);

            double r = ((double) n1) / n2;
            TreeMap<Integer, Double[]> treemap = getSegz(corpus, 2, h, 0);           
            regOutput(treemap, r, 1);
        }
        
        else if(flag == 500){
            int h1 = Integer.valueOf(args[1]);
            int h2 = Integer.valueOf(args[2]);
            int w = Integer.valueOf(args[3]);
            int count = 0;
            ArrayList<Integer> words_after_h1h2 = new ArrayList<Integer>();
            for (int i = 0; i < corpus.size() - 2; i++) {
            	if (corpus.get(i) == h1 && corpus.get(i + 1) == h2) {
            		words_after_h1h2.add(corpus.get(i+2));
            		if (corpus.get(i+2) == w) {
            			count++;
            		}
            	}
            }
            //output 
            System.out.println(count);
            System.out.println(words_after_h1h2.size());
            if(words_after_h1h2.size() == 0)
                System.out.println("undefined");
            else
                System.out.println(String.format("%.7f",count/(double)words_after_h1h2.size()));
        }
        else if(flag == 600){
            int n1 = Integer.valueOf(args[1]);
            int n2 = Integer.valueOf(args[2]);
            int h1 = Integer.valueOf(args[3]);
            int h2 = Integer.valueOf(args[4]);
            double r = ((double) n1)/n2;
            TreeMap<Integer, Double[]> treemap = getSegz(corpus, 3, h1, h2);
            if (treemap.size() == 0) {
            	System.out.println("undefined");
            	return;
            }
            regOutput(treemap, r, 1);
        }
        else if(flag == 700){
            int seed = Integer.valueOf(args[1]);
            int t = Integer.valueOf(args[2]);
            int h1=0,h2=0;
            TreeMap<Integer, Double[]> treemap1;
            TreeMap<Integer, Double[]> treemap2;
            TreeMap<Integer, Double[]> treemap3;
            Random rng = new Random();
            if (seed != -1) rng.setSeed(seed);
            if(t == 0){
                double r = rng.nextDouble();
                
                treemap1 = getSegz(corpus, 1, 0, 0);
                h1 = regOutput(treemap1, r, 0);
                System.out.println(h1);
                if(h1 == 9 || h1 == 10 || h1 == 12){
                    return;
                }
                
                r = rng.nextDouble();
                treemap2 = getSegz(corpus, 2, h1, 0);
                h2 = regOutput(treemap2, r, 0);
                System.out.println(h2);
            }
            else if(t == 1){
                h1 = Integer.valueOf(args[3]);
                
                double r = rng.nextDouble();
                treemap2 = getSegz(corpus, 2, h1, 0);
                h2 = regOutput(treemap2, r, 0);
                System.out.println(h2);
            }
            else if(t == 2){
                h1 = Integer.valueOf(args[3]);
                h2 = Integer.valueOf(args[4]);
            }

            while(h2 != 9 && h2 != 10 && h2 != 12){
                double r = rng.nextDouble();
                int w  = 0;
                
                treemap3 = getSegz(corpus, 3, h1, h2);
                if (treemap3.size() == 0) {
                	treemap2 = getSegz(corpus, 2, h2, 0);
                	w = regOutput(treemap2, r, 0);
                }
                else {
                	w = regOutput(treemap3, r, 0);
                }
                System.out.println(w);
                h1 = h2;
                h2 = w;
            }
        }

        return;
    }
}

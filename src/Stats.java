import java.io.FileReader;
import java.io.IOException;
import java.io.BufferedReader;
import java.util.ArrayList;

import org.apache.commons.math3.distribution.TDistribution;

public class Stats {
	public static void main(String[] args) throws IOException{
		BufferedReader input = new BufferedReader(new FileReader(args[0]));
		String line = input.readLine();
		ArrayList<Double> samplesList = new ArrayList<Double>();

		while (line != null) {
			samplesList.add(Double.parseDouble(line));
			line = input.readLine();
		}
		double[] samples = new double[samplesList.size()];
		for(int i = 0; i < samplesList.size(); i++) {
			samples[i] = samplesList.get(i);
			//System.out.println("samples[" + i + "] = " + samples[i]);
		}
		double[] interval = confidenceInterval(samples, 0.95);
		System.out.println("Intervalo: " + interval[0] + " - " + interval[1] + " - " + interval[2]);
	}

	private static double calcAverage(double[] samples) {
		double sum = 0.0;

		for (double sample : samples)
			sum += sample;

		return sum/samples.length;
	}

	//Calculo do desvio padrao
	private static double calcStandardDeviation(double[] samples, double average) {
		double sum = 0.0;

		for (double sample : samples)
			sum += (sample-average)*(sample-average);

		return Math.sqrt(sum/samples.length);
	}

	//Calculo do intervalo de confiança
	private static double[] confidenceInterval(double[] samples, double confidence) {
		double average = calcAverage(samples);
		double standardDeviation = calcStandardDeviation(samples, average);
		double n = samples.length;
		TDistribution studentT = new TDistribution(n);
		double t = studentT.density(1 - confidence);
		double delta = t * standardDeviation / Math.sqrt(n);

		return new double[] { average - delta, average, average + delta };
	}
}



package levocon.controller;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.util.Calendar;
import java.util.Date;

import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.SpinnerDateModel;
import javax.swing.border.EmptyBorder;

import com.xeiam.xchart.Chart;
import com.xeiam.xchart.QuickChart;
import com.xeiam.xchart.XChartPanel;

public class HistoryActivity extends JFrame {

	private static final long serialVersionUID = -6785316692965043443L;
	private JPanel contentPane;
	private JPanel pnlChartArea;
	private JPanel pnlControls;
	private JLabel lblShowingDataFrom;
	private JLabel lblSensorsFrom;
	private JLabel lblTo;
	private JComboBox comboBox;
	
	private NetworkHandler nh;

	/**
	 * Create the frame.
	 */
	public HistoryActivity(NetworkHandler nh) {
		// SET UP A NETWORK HANDLER
		this.nh = nh;
		
		// CREATE WINDOW
		// Set window options
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setPreferredSize(new Dimension(800, 600));
		setBounds(150, 150, 800, 600);
		setTitle("Historic data");

		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		contentPane.setLayout(new BorderLayout(0, 0));

		// Show the controls
		showControls();

		// Show the graph
		showGraph();
	}

	/**
	 * Show controls
	 */
	public void showControls() {
		// Set up the pane
		pnlControls = new JPanel();
		contentPane.add(pnlControls, BorderLayout.NORTH);
		pnlControls.setLayout(new BoxLayout(pnlControls, BoxLayout.X_AXIS));

		// Topic selector
		lblShowingDataFrom = new JLabel("Showing data from ");
		pnlControls.add(lblShowingDataFrom);

		comboBox = new JComboBox();
		comboBox.setModel(new DefaultComboBoxModel(new String[] {
				"temperature", "humidity", "CO2 concentration" }));
		pnlControls.add(comboBox);

		// From selector
		lblSensorsFrom = new JLabel(" sensors from ");
		pnlControls.add(lblSensorsFrom);

		Calendar cal = Calendar.getInstance(); // Get current time
		Date today = cal.getTime();
		cal.add(Calendar.DATE, -1);
		Date yesterday = cal.getTime();

		JSpinner spinnerFrom = new JSpinner(new SpinnerDateModel());
		JSpinner.DateEditor editorFrom = new JSpinner.DateEditor(spinnerFrom,
				"dd/MM/yyyy HH:mm");
		spinnerFrom.setEditor(editorFrom);
		spinnerFrom.setValue(yesterday); // Standard value: yesterday
		pnlControls.add(spinnerFrom);

		// To selector
		lblTo = new JLabel(" to ");
		pnlControls.add(lblTo);

		pnlChartArea = new JPanel();
		contentPane.add(pnlChartArea, BorderLayout.CENTER);
		pnlChartArea.setLayout(new BoxLayout(pnlChartArea, BoxLayout.X_AXIS));

		JSpinner spinnerTo = new JSpinner(new SpinnerDateModel());
		JSpinner.DateEditor editorTo = new JSpinner.DateEditor(spinnerTo,
				"dd/MM/yyyy HH:mm");
		spinnerTo.setEditor(editorTo);
		spinnerTo.setValue(today); // Standard value: now
		pnlControls.add(spinnerTo);
	}

	/**
	 * Show the graph
	 */
	public void showGraph() {
		double[] xData = new double[] { 0, 1, 2, 3, 4, 5 };
		double[] yData = new double[] { 2.0, 1.5, 2.4, 5.5, 2.2, 1.3 };

		// Create Chart
		Chart chart = QuickChart.getChart("Temperature data", "Time",
				"Temperature", "Sensor 1", xData, yData);

		// Show it
		JPanel pnlChart = new XChartPanel(chart);
		pnlChartArea.add(pnlChart);
		pnlChartArea.validate();
	}

}
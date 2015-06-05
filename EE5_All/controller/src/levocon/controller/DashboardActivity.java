package levocon.controller;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import javax.swing.Box;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;

public class DashboardActivity extends JFrame {

	private static final long serialVersionUID = -4778980660902943160L;
	private JPanel contentPane;
	private NetworkHandler nh;

	/**
	 * Create the frame.
	 */
	public DashboardActivity(NetworkHandler nh) {
		// SET UP A NETWORK HANDLER
		this.nh = nh;
		
		// CREATE WINDOW
		// Set window options
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setPreferredSize(new Dimension(600, 300));
		setBounds(150, 150, 600, 300);
		setTitle("Dashboard");

		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		GridBagLayout gbl_contentPane = new GridBagLayout();
		gbl_contentPane.columnWidths = new int[] { 0, 0, 0, 0, 0 };
		gbl_contentPane.rowHeights = new int[] { 0, 0, 0, 0 };
		gbl_contentPane.columnWeights = new double[] { 0.0, 0.0, 0.0, 0.0,
				Double.MIN_VALUE };
		gbl_contentPane.rowWeights = new double[] { 0.0, 0.0, 0.0,
				Double.MIN_VALUE };
		contentPane.setLayout(gbl_contentPane);

		Component strutWindowMarginHorizontal = Box.createHorizontalStrut(20);
		GridBagConstraints gbc_strutWindowMarginHorizontal = createGbc(0, 0);
		contentPane.add(strutWindowMarginHorizontal,
				gbc_strutWindowMarginHorizontal);

		Component strutLabelMargin = Box.createHorizontalStrut(20);
		GridBagConstraints gbc_strutLabelMargin = createGbc(2, 0);
		contentPane.add(strutLabelMargin, gbc_strutLabelMargin);

		Component strutWindowMarginVertical = Box.createVerticalStrut(20);
		GridBagConstraints gbc_strutWindowMarginVertical = createGbc(0, 0);
		contentPane.add(strutWindowMarginVertical,
				gbc_strutWindowMarginVertical);

		JLabel lblTemperature = new JLabel("Temperature");
		lblTemperature.setFont(new Font("Dialog", Font.PLAIN, 20));
		GridBagConstraints gbc_lblTemperature = createGbc(1, 1);
		contentPane.add(lblTemperature, gbc_lblTemperature);

		JLabel lblTemperatureNumber = new JLabel("15.4 °C");
		lblTemperatureNumber.setFont(new Font("Dialog", Font.BOLD, 20));
		GridBagConstraints gbc_lblTemperatureNumber = createGbc(3, 1);
		contentPane.add(lblTemperatureNumber, gbc_lblTemperatureNumber);

		JLabel lblHumidity = new JLabel("Humidity");
		lblHumidity.setFont(new Font("Dialog", Font.PLAIN, 20));
		GridBagConstraints gbc_lblHumidity = createGbc(1, 2);
		contentPane.add(lblHumidity, gbc_lblHumidity);

		JLabel lblHumidityNumber = new JLabel("54%");
		lblHumidityNumber.setFont(new Font("Dialog", Font.BOLD, 20));
		GridBagConstraints gbc_lblHumidityNumber = createGbc(3, 2);
		contentPane.add(lblHumidityNumber, gbc_lblHumidityNumber);
	}

	/**
	 * Returns GridBagConstraints object for the dashboard
	 * 
	 * @param x
	 *            - X coordinate in the grid
	 * @param y
	 *            - Y coordinate in the grid
	 * @return Generated GridBagConstraints
	 */
	private GridBagConstraints createGbc(int x, int y) {
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = x;
		gbc.gridy = y;
		gbc.fill = GridBagConstraints.BOTH;
		gbc.insets = new Insets(5, 5, 5, 5);
		return gbc;
	}
	
	public void updateTopic(Topic topic){
		// TODO Have auto-updating values
		//((MessageSensor) topic.getLatestMessage()).getValue();
	}

}
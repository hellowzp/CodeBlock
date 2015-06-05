package levocon.controller;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.SpringLayout;
import javax.swing.SwingConstants;
import javax.swing.border.EmptyBorder;

public class MainActivity extends JFrame implements ActionListener {
	
	private NetworkHandler nh;
	
	private static final long serialVersionUID = 5481876836542227705L;
	private JPanel contentPane;
	private JCheckBox chckbxEnableNetwork;
	private JButton btnShowDashboard;
	private JButton btnShowHistoricData;
	private JButton btnManageSensors;
	private JCheckBox chckbxAutoSubscribe;
	private JProgressBar progressBar;
	private JPanel statusBar;
	
	private JLabel lblConnectedSensorsNumber;
	private JLabel lblSubscribedSensorsNumber;
	
	/**
	 * Create the frame.
	 */
	public MainActivity(NetworkHandler nh) {
		
		// SET UP A NETWORK HANDLER
		this.nh = nh;
		
		// CREATE WINDOW
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setTitle("Levocon");
		
		setBounds(100, 100, 450, 300);
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		contentPane.setLayout(new BorderLayout(0, 0));
		setContentPane(contentPane);
		
		// NORTH WINDOW CONTENT
		chckbxEnableNetwork = new JCheckBox("Enable wireless sensor network");
		chckbxEnableNetwork.setHorizontalAlignment(SwingConstants.TRAILING);
		chckbxEnableNetwork.setActionCommand("network");
		chckbxEnableNetwork.addActionListener(this);
		contentPane.add(chckbxEnableNetwork, BorderLayout.NORTH);
		
		// SOUTH WINDOW CONTENT
		Box verticalBox = Box.createVerticalBox();
		contentPane.add(verticalBox, BorderLayout.SOUTH);
		
		progressBar = new JProgressBar();
		progressBar.setString("Connecting...");
		progressBar.setStringPainted(true);
		progressBar.setIndeterminate(true);
		
		statusBar = new JPanel();
		statusBar.setLayout(new BoxLayout(statusBar, BoxLayout.X_AXIS));
		
		JLabel lblConnectedSensors = new JLabel("Connected: ");
		statusBar.add(lblConnectedSensors);
		
		lblConnectedSensorsNumber = new JLabel("0");
		statusBar.add(lblConnectedSensorsNumber);
		
		Component strutConnectedSensors = Box.createHorizontalStrut(60);
		statusBar.add(strutConnectedSensors);
		
		JLabel lblSubscribedSensors = new JLabel("Subscribed: ");
		statusBar.add(lblSubscribedSensors);
		
		lblSubscribedSensorsNumber = new JLabel("0");
		statusBar.add(lblSubscribedSensorsNumber);
		
		Component strutSubscribedSensors = Box.createHorizontalStrut(60);
		statusBar.add(strutSubscribedSensors);
		
		chckbxAutoSubscribe = new JCheckBox("Auto-subscribe");
		chckbxAutoSubscribe.setActionCommand("network");
		chckbxAutoSubscribe.addActionListener(this);
		statusBar.add(chckbxAutoSubscribe);
		
		// CENTER WINDOW CONTENT
		JPanel panel = new JPanel();
		contentPane.add(panel, BorderLayout.CENTER);
		SpringLayout sl_panel = new SpringLayout();
		panel.setLayout(sl_panel);
		
		btnShowDashboard = new JButton("Show current sensor readings");
		btnShowDashboard.setEnabled(false);
		sl_panel.putConstraint(SpringLayout.NORTH, btnShowDashboard, 46, SpringLayout.NORTH, panel);
		sl_panel.putConstraint(SpringLayout.WEST, btnShowDashboard, 0, SpringLayout.WEST, panel);
		sl_panel.putConstraint(SpringLayout.EAST, btnShowDashboard, 440, SpringLayout.WEST, panel);
		btnShowDashboard.setActionCommand("dashboard");
		btnShowDashboard.addActionListener(this);
		panel.add(btnShowDashboard);
		
		btnShowHistoricData = new JButton("Show historic data graphs");
		btnShowHistoricData.setEnabled(false);
		sl_panel.putConstraint(SpringLayout.NORTH, btnShowHistoricData, 6, SpringLayout.SOUTH, btnShowDashboard);
		sl_panel.putConstraint(SpringLayout.WEST, btnShowHistoricData, 0, SpringLayout.WEST, btnShowDashboard);
		sl_panel.putConstraint(SpringLayout.EAST, btnShowHistoricData, 440, SpringLayout.WEST, panel);
		btnShowHistoricData.setActionCommand("graphs");
		btnShowHistoricData.addActionListener(this);
		panel.add(btnShowHistoricData);
		
		btnManageSensors = new JButton("Manage sensors");
		btnManageSensors.setEnabled(false);
		sl_panel.putConstraint(SpringLayout.NORTH, btnManageSensors, 6, SpringLayout.SOUTH, btnShowHistoricData);
		sl_panel.putConstraint(SpringLayout.WEST, btnManageSensors, 0, SpringLayout.WEST, btnShowDashboard);
		sl_panel.putConstraint(SpringLayout.EAST, btnManageSensors, 0, SpringLayout.EAST, btnShowDashboard);
		btnManageSensors.setActionCommand("manage");
		btnManageSensors.addActionListener(this);
		panel.add(btnManageSensors);
	}
	
	/**
	 * Handle button presses
	 */
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("graphs")) {
			nh.setHistoryActivity(new HistoryActivity(nh));
			JFrame newWindow = nh.getHistoryActivity();
			newWindow.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
			newWindow.pack();
			newWindow.setVisible(true);
		} else if (e.getActionCommand().equals("dashboard")) {
			nh.setDashboardActivity(new DashboardActivity(nh));
			JFrame newWindow = nh.getDashboardActivity();
			newWindow.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
			newWindow.pack();
			newWindow.setVisible(true);
		} else if (e.getActionCommand().equals("network")) {
			toggleNetwork();
		}
	}
	
	private void toggleNetwork() {
		if (chckbxEnableNetwork.isSelected()) {
			// TODO ENABLE NETWORK
			
			// UPDATE UI
			btnShowDashboard.setEnabled(true);
			btnShowHistoricData.setEnabled(true);
			btnManageSensors.setEnabled(true);
			contentPane.add(progressBar, BorderLayout.SOUTH);
			contentPane.validate();
			contentPane.repaint();
		} else {
			// TODO DISABLE NETWORK
			
			// UPDATE UI
			btnShowDashboard.setEnabled(false);
			btnShowHistoricData.setEnabled(false);
			btnManageSensors.setEnabled(false);
			contentPane.remove(progressBar);
			contentPane.remove(statusBar);
			contentPane.validate();
			contentPane.repaint();
		}
	}
	
	/**
	 * Update info
	 */
	public void updateActivity() {
		//Show progress or status bar
		int connectedTopics = nh.getConnectedTopics().size();
		int subscribedTopics = nh.getSubscribedTopics().size();
		
		contentPane.remove(progressBar);
		contentPane.remove(statusBar);
		if(connectedTopics > 0){
			contentPane.add(statusBar, BorderLayout.SOUTH);
		} else{
			contentPane.add(progressBar, BorderLayout.SOUTH);
		}
		
		//Update number of connected and subscribed topics
		lblConnectedSensorsNumber.setText(Integer.toString(connectedTopics));
		lblSubscribedSensorsNumber.setText(Integer.toString(subscribedTopics));
		
		//Update view!
		contentPane.validate();
		contentPane.repaint();
	}
	
}
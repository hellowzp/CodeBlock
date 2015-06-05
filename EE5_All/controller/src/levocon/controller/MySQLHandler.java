package levocon.controller;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;



public class MySQLHandler {
	
	Connection con = null;
	String url = "199.195.144.168";
	String db = "ee5";
	String driver = "com.mysql.jdbc.Driver";
	String user = "ee5";
	String pass = "a*nGC]YE?UC{";
	
	
	MySQLHandler(){
		try {
			Class.forName(driver).newInstance();
		} catch (InstantiationException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception in the MySQLHandler!");
		} catch (IllegalAccessException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception in the MySQLHandler!");
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception in the MySQLHandler!");
		}
	}
	
	public void runQuery(String topicName, int value){
		try {
			con = DriverManager.getConnection("jdbc:mysql://" + url + "/" + db, user, pass);
			Statement statement = con.createStatement();
			statement.executeUpdate("INSERT INTO log_temperature (topicName, temperature) VALUES ('" + topicName + "', " + value + ");");
			con.close();
		} catch (SQLException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception in the MySQLHandler!");
		}
	}
	
}
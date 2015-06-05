package levocon.controller;

import java.nio.ByteBuffer;
import java.util.Random;

public class TestPublisher implements Runnable {
	
	NetworkHandler nh;
	int topicId;
	
	TestPublisher(NetworkHandler nh, int topicId) {
		this.nh = nh;
		this.topicId = topicId;
		
	}
	
	public void run() {
		while (true) {
			//Publish a message every 10 seconds
			MessageOut publishMessage = new MessageOut(topicId, MessageType.PUBLISH, ReturnValue.ACCEPTED, false, false, false, false);
			Random random = new Random();
			int temperature = (random.nextInt(300)); //Generate temperature between 0 and 30 degrees
			temperature = Math.round(temperature);
			byte[] byteTemperature = ByteBuffer.allocate(4).putInt(temperature).array();
			publishMessage.setMessage(byteTemperature);
			nh.send(publishMessage);
			try {
				Thread.sleep(10000);
			} catch (InterruptedException e) {
				e.printStackTrace();
				System.err.println("ERROR: TestPublisher object has insomnia...");
			}
		}
	}
	
}

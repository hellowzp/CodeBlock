package levocon.controller;

public class MessageSensor extends Message {
	
	private int value;
	
	public MessageSensor(int topicId, MessageType messageType, ReturnValue returnValue, long length) {
		super(topicId, messageType, returnValue, length);
	}
	
	public int getValue() {
		return value;
	}
	
	public void setMessage(byte[] message) {
		super.setMessage(message);
		value = ((message[0] & 0xff) << 24) | ((message[1] & 0xff) << 16) | ((message[2] & 0xff) << 8) | (message[3] & 0xff);
	}
	
}

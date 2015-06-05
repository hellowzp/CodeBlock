package levocon.controller;

import java.io.UnsupportedEncodingException;

public class Message {
	
	private int topicId;
	private MessageType messageType;
	private ReturnValue returnValue;
	private long length;
	private byte[] message;
	
	public Message(int topicId, MessageType messageType, ReturnValue returnValue, long length) {
		this.topicId = topicId;
		this.messageType = messageType;
		this.returnValue = returnValue;
		this.length = length;
		this.message = new byte[0];
	}
	
	public int getTopicId() {
		return topicId;
	}
	
	public MessageType getMessageType() {
		return messageType;
	}
	
	public ReturnValue getReturnValue() {
		return returnValue;
	}
	
	public long getLength() {
		return length;
	}
	
	public byte[] getMessage() {
		return message;
	}
	
	public void setMessage(byte[] message) {
		this.message = message;
		length = message.length;
	}
	
	public String getMessageString() {
		try {
			String string = new String(message, "US-ASCII");
			return string;
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception while converting byte[] to String!");
			return null;
		}
	}
	
	public void setMessageString(String string) {
		try {
			message = string.getBytes("US-ASCII");
			length = message.length;
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception while converting String to byte[]!");
		}
	}
	
}
package levocon.controller;

public class MessageOut extends Message {
	
	private long address; // KNOWN ISSUE: long is signed in Java!
	private boolean duplicate;
	private boolean will;
	private boolean clean;
	private boolean retain;
	
	public MessageOut(int topicId, MessageType messageType, ReturnValue returnValue, boolean duplicate, boolean will, boolean clean, boolean retain) {
		super(topicId, messageType, returnValue, 0);
		this.address = 0;
		this.duplicate = duplicate;
		this.will = will;
		this.clean = clean;
		this.retain = retain;
	}
	
	public long getAddress() {
		return address;
	}
	
	public int getDuplicate() {
		return duplicate ? 1 : 0;
	}
	
	public int getWill() {
		return will ? 1 : 0;
	}
	
	public int getClean() {
		return clean ? 1 : 0;
	}
	
	public int getRetain() {
		return retain ? 1 : 0;
	}
	
}

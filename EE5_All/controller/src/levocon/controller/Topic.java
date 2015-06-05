package levocon.controller;

import java.util.ArrayList;
import java.util.List;

public class Topic {

    private int topicId;
	private String topicName;
	private TopicType topicType;
	private List<Message> messages;

	public Topic(int topicId, String topicName, TopicType topicType) {
		this.topicId = topicId;
		this.topicName = topicName;
		this.topicType = topicType;
		messages = new ArrayList<Message>();
	}

	public int getTopicId() {
		return topicId;
	}
	
	public String getTopicName(){
		return topicName;
	}
	
	public TopicType getTopicType(){
		return topicType;
	}
	
	public void addMessage(Message m){
		messages.add(m);
	}
	
	public Message getLatestMessage(){
		return messages.get((messages.size())-1);
	}

	public void update(Message message) {
		// Get and process latest message
		if(message.getTopicId() == topicId){
			// Add to list of received messages
			addMessage(message);
			
			// DEBUG
			System.out.println("LOG: message received!");
			System.out.println("    Message topic: " + topicName + " (" + topicId + ")");
			System.out.println("    Message content: " + message.getMessage());
		}
	}

}
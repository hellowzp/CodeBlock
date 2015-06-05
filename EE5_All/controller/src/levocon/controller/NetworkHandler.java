package levocon.controller;

import java.awt.EventQueue;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class NetworkHandler implements Runnable {
	
	// FIFO communication definitions
	private static final String dbName = "mqtt_db.db";
	private static final String fifoFolder = "tmp";
	private static final String fifoIn = fifoFolder + "/bridgetojava";
	private static final String fifoOut = fifoFolder + "/javatobridge";
	private static final String execPath = "bridge/start_process.sh " + dbName;
	private static final int messageHeaderLength = 8;
	private Process process;
	
	//MySQL handler
	private MySQLHandler mysqlHandler;
	
	// Application windows
	private MainActivity mainActivity;
	private DashboardActivity dashboardActivity;
	private HistoryActivity historyActivity;
	
	// Network variables
	private int controllerTopicId;
	private List<String> connectedTopics = new ArrayList<String>();
	private List<Topic> subscribedTopics = new ArrayList<Topic>();
	private boolean autoSubscribe;
	private String subscribeTopicName;
	private String unsubscribeTopicName;
	
	// Network definitions
	private static final String controllerName = "controller";
	private static final String controllerTopicName = "controller";
	private static final String connectTopicName = "connect";
	private static final String disconnectTopicName = "disconnect";
	private static final String resultTopicName = "result";
	private static final String cloudTopicName = "controller";
	private static final String tempTopicName = "wasp/temp";
	
	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					// Start a network handler
					NetworkHandler nh = new NetworkHandler();
					new Thread(nh).start();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}
	
	public NetworkHandler() {
		// Set up subscription system
		autoSubscribe = true;
		subscribeTopicName = "";
		unsubscribeTopicName = "";
		
		// Set up MySQL handler
		mysqlHandler = new MySQLHandler();
		
		// Initialize the network
		init();
		connect(); // DEBUG
		
		// Launch the main menu
		mainActivity = new MainActivity(this);
		mainActivity.setVisible(true);
	}
	
	/**
	 * NETWORK PREFERENCES - Toggle auto subscribe
	 */
	public void setAutoSubscribe(boolean autoSubscribe) {
		this.autoSubscribe = autoSubscribe;
	}
	
	public List<String> getConnectedTopics() {
		return connectedTopics;
	}
	
	public List<Topic> getSubscribedTopics() {
		return subscribedTopics;
	}
	
	/**
	 * Get a subscribed topic by its name
	 */
	public Topic getTopicByName(String topicName) {
		for (Topic topic : subscribedTopics) {
			if (topic.getTopicName().equals(topicName)) {
				return topic;
			}
		}
		return null;
	}
	
	/**
	 * Get a subscribed topic by its ID
	 */
	public Topic getTopicById(int topicId) {
		for (Topic topic : subscribedTopics) {
			if (topic.getTopicId() == topicId) {
				return topic;
			}
		}
		return null;
	}
	
	public DashboardActivity getDashboardActivity() {
		return dashboardActivity;
	}
	
	public void setDashboardActivity(DashboardActivity dashboardActivity) {
		this.dashboardActivity = dashboardActivity;
	}
	
	public HistoryActivity getHistoryActivity() {
		return historyActivity;
	}
	
	public void setHistoryActivity(HistoryActivity historyActivity) {
		this.historyActivity = historyActivity;
	}
	
	public void notifyActivities() {
		//mainActivity.updateData();
		//dashboardActivity.updateData();
		//historyActivity.updateData();
	}
	
	/**
	 * NETWORK SETUP - Run the C programs, create FIFOs, attach shutdown hooks
	 */
	public void init() {
		// Initialize FIFOs
		Runtime runtime = Runtime.getRuntime();
		try {
			runtime.exec("mkdir " + fifoFolder);
			runtime.exec("mkfifo --mode=0666 " + fifoIn);
			runtime.exec("mkfifo --mode=0666 " + fifoOut);
			process = runtime.exec(execPath);
			NetworkKiller killer = new NetworkKiller();
			killer.attachKiller(runtime);
		} catch (IOException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception while launching C programs!");
		}
	}
	
	/**
	 * NETWORK SETUP - Send a connection request
	 */
	public void connect() {
		MessageOut connectMessage = new MessageOut(0, MessageType.CONNECT, ReturnValue.ACCEPTED, false, false, false, false);
		connectMessage.setMessageString(controllerName);
		send(connectMessage);
	}
	
	/**
	 * NETWORK SETUP - Request a topic name
	 */
	public void register() {
		MessageOut registerMessage = new MessageOut(0, MessageType.REGISTER, ReturnValue.ACCEPTED, false, false, false, false);
		registerMessage.setMessageString(controllerTopicName);
		send(registerMessage);
	}
	
	/**
	 * SUBSCRIPTIONS - Request subscribe to topic
	 */
	public void subscribe(String topicName) {
		if (subscribeTopicName.isEmpty()) {
			MessageOut subscribeMessage = new MessageOut(controllerTopicId, MessageType.SUBSCRIBE, ReturnValue.ACCEPTED, false, false, false, false);
			subscribeMessage.setMessageString(topicName);
			subscribeTopicName = topicName;
			send(subscribeMessage);
		} else {
			System.err.println("ERROR: another subscription request is still pending");
		}
	}
	
	/**
	 * SUBSCRIPTIONS - Request unsubscribe from topic
	 */
	public void unsubscribe(String topicName) {
		if (unsubscribeTopicName.isEmpty()) {
			MessageOut unsubscribeMessage = new MessageOut(controllerTopicId, MessageType.UNSUBSCRIBE, ReturnValue.ACCEPTED, false, false, false, false);
			unsubscribeMessage.setMessageString(topicName);
			unsubscribeTopicName = topicName;
			send(unsubscribeMessage);
		} else {
			System.err.println("ERROR: another subscription request is still pending");
		}
	}
	
	/**
	 * SUBSCRIPTIONS - Add topic to subscribed topics
	 */
	public void attach(Topic topic) {
		subscribedTopics.add(topic);
	}
	
	/**
	 * SUBSCRIPTIONS - Delete topic from subscribed topics
	 */
	public void detach(Topic topic) {
		subscribedTopics.remove(topic);
	}
	
	/**
	 * RECEIVE MESSAGES - Stream incoming messages
	 */
	public void run() {
		try {
			// Open FIFO (read sync)
			BufferedInputStream in = new BufferedInputStream(new FileInputStream(fifoIn));
			
			// Declare some local variables
			byte[] header = new byte[messageHeaderLength];
			int topicId;
			MessageType messageType;
			ReturnValue returnValue;
			long length;
			Message message = null;
			
			// Stream data from FIFO
			while (true) {
				if (in.available() >= messageHeaderLength) {
					// Get message header;
					// WARNING: x86 systems are little endian, LSByte comes first!
					in.read(header);
					topicId = ((header[1] & 0xff) << 8) | (header[0] & 0xff);
					messageType = MessageType.values()[(header[2] & 0xff)];
					returnValue = ReturnValue.values()[(header[3] & 0xff)];
					length = ((header[7] & 0xffL) << 32) | ((header[6] & 0xffL) << 16) | ((header[5] & 0xffL) << 8) | (header[4] & 0xffL);
					
					// Create the correct kind of message
					if (getTopicById(topicId) != null) {
						switch (getTopicById(topicId).getTopicType()) {
							case SENSOR:
								message = new MessageSensor(topicId, messageType, returnValue, length);
								break;
							case RESULT:
								// TODO Implement result messages for HistoryActivity
							default:
								message = new Message(topicId, messageType, returnValue, length);
								break;
						}
					} else {
						message = new Message(topicId, messageType, returnValue, length);
					}
					
					// Get message body
					if (length > 0L) {
						byte[] byteMessage = new byte[(int) length];
						while (in.available() < length) {
							// Wait till the entire message is available
							Thread.sleep(1000);
						}
						in.read(byteMessage);
						message.setMessage(byteMessage);
					}
					
					// Notify subscribed topics of new message
					notifyTopics(message);
					
					//Handle the message
					handleMessage(message);
				}
				Thread.sleep(1000);
			}
		} catch (IOException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception while streaming messages!");
		} catch (InterruptedException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception while streaming messages!");
		}
	}
	
	/**
	 * RECEIVE MESSAGES - Update subscribed topics
	 */
	public void notifyTopics(Message message) {
		for (Topic topic : subscribedTopics) {
			topic.update(message);
		}
	}
	
	/**
	 * RECEIVE MESSAGES - Handle new message on topic
	 */
	public void handleMessage(Message message) {
		switch (message.getMessageType()) {
			case PUBLISH:
				Topic topic = getTopicById(message.getTopicId());
				if (topic != null) {
					switch (getTopicById(message.getTopicId()).getTopicType()) {
						case SENSOR:
							notifyDashboardActivity();
							
							//Upload to NxWeb if necessary
							String safeTopicName = topic.getTopicName().substring(0,10);
							if(topic.getTopicName().equals(cloudTopicName)){
								mysqlHandler.runQuery(topic.getTopicName(), ((MessageSensor) message).getValue());
							} else if(safeTopicName.equals(tempTopicName)){
								mysqlHandler.runQuery(safeTopicName, ((MessageSensor) message).getValue());
							}
							
							break;
						case RESULT:
							// TODO Update graph? Wait till last message is received first?
							break;
						case CONNECT:
							connectedTopics.add(message.getMessageString());
							if (autoSubscribe) {
								System.out.println("LOG: subscribing to topic: " + message.getMessageString());
								subscribe(message.getMessageString());
							}
							break;
						case DISCONNECT:
							connectedTopics.remove(message.getMessageString());
							break;
					}
				} else{
                    // TODO Handle this better
					System.out.println("LOG: received PUBLISH from old subscribed topic, ignored.");
				}
				break;
			case CONNACK:
				// Connect acknowledgement, register topic name if accepted
				if (message.getReturnValue() == ReturnValue.ACCEPTED) {
					notifyMainActivity();
					System.out.println("LOG: connect acknowledgement!");
					register();
				} else {
					System.err.println("ERROR: connection to network failed!");
				}
				break;
			case REGACK:
				// Register acknowledgement, save topic ID if accepted and subscribe to connect topic
				if (message.getReturnValue() == ReturnValue.ACCEPTED) {
					notifyMainActivity();
					System.out.println("LOG: register acknowledgement");
					controllerTopicId = message.getTopicId();
					connectedTopics.add("Test");
					
					//DEBUG: start publishing on own topic
					TestPublisher pub = new TestPublisher(this, controllerTopicId);
					new Thread(pub).start();
					
					subscribe(connectTopicName);
				} else {
					System.err.println("ERROR: registering controller's topic name failed");
				}
				break;
			case SUBACK:
				// Subscribe acknowledgement, put topic in subscribed topics
				if (message.getReturnValue() == ReturnValue.ACCEPTED) {
					notifyMainActivity();
					System.out.println("LOG: subscribe acknowledgement for " + subscribeTopicName);
					Topic subscribeTopic;
					if (subscribeTopicName.equals(connectTopicName)) {
						subscribeTopic = new Topic(message.getTopicId(), subscribeTopicName, TopicType.CONNECT);
					} else if (subscribeTopicName.equals(disconnectTopicName)) {
						subscribeTopic = new Topic(message.getTopicId(), subscribeTopicName, TopicType.DISCONNECT);
					} else {
						subscribeTopic = new Topic(message.getTopicId(), subscribeTopicName, TopicType.SENSOR);
					}
					
					attach(subscribeTopic);
					subscribeTopicName = "";
					
					//If SUBACK comes from connect topic, subscribe to disconnect topic
					//DEBUG: if SUBACK comes from disconnect topic, subscribe to own topic
					if (subscribeTopic.getTopicType() == TopicType.CONNECT) {
						subscribe(disconnectTopicName);
					} else if (subscribeTopic.getTopicType() == TopicType.DISCONNECT) {
						subscribe(controllerTopicName);
					}
				} else {
					System.err.println("ERROR: subscribing to topic failed!");
				}
				break;
			case UNSUBACK:
				// Unsubscribe acknowledgement, delete topic from subscribed topics
				if (message.getReturnValue() == ReturnValue.ACCEPTED) {
					notifyMainActivity();
					System.out.println("LOG: unsubscribe acknowledgement for " + unsubscribeTopicName);
					subscribedTopics.remove(getTopicByName(unsubscribeTopicName));
					unsubscribeTopicName = "";
				} else {
					System.err.println("ERROR: unsubscribing from topic failed!");
				}
				break;
		}
	}
	
	public void notifyMainActivity() {
		mainActivity.updateActivity();
	}
	
	public void notifyDashboardActivity() {
		//dashboardActivity.updateActivity();
	}
	
	/**
	 * SEND MESSAGES - Sends a message to the bridge
	 */
	public void send(MessageOut message) {
		BufferedOutputStream out;
		try {
			// Open FIFO (write sync)
			out = new BufferedOutputStream(new FileOutputStream(fifoOut));
			
			// Construct the header
			// WARNING: x86 systems are little endian, LSByte comes first!
			byte[] byteAddress = ByteBuffer.allocate(8).putLong(message.getAddress()).array();
			byte[] byteTopicId = ByteBuffer.allocate(2).putShort((short) message.getTopicId()).array();
			byte[] byteMessageType = ByteBuffer.allocate(1).put((byte) message.getMessageType().ordinal()).array();
			byte[] byteDuplicate = ByteBuffer.allocate(1).put((byte) message.getDuplicate()).array();
			byte[] byteWill = ByteBuffer.allocate(1).put((byte) message.getWill()).array();
			byte[] byteClean = ByteBuffer.allocate(1).put((byte) message.getClean()).array();
			byte[] byteRetain = ByteBuffer.allocate(1).put((byte) message.getRetain()).array();
			byte[] byteReturnValue = ByteBuffer.allocate(1).put((byte) message.getReturnValue().ordinal()).array();
			byte[] byteLength = ByteBuffer.allocate(4).putInt((int) message.getLength()).array();
			
			// Endian conversion
			byteAddress = reverseBytes(byteAddress);
			byteTopicId = reverseBytes(byteTopicId);
			byteMessageType = reverseBytes(byteMessageType);
			byteDuplicate = reverseBytes(byteDuplicate);
			byteWill = reverseBytes(byteWill);
			byteClean = reverseBytes(byteClean);
			byteRetain = reverseBytes(byteRetain);
			byteReturnValue = reverseBytes(byteReturnValue);
			byteLength = reverseBytes(byteLength);
			
			// Write the message
			out.write(byteAddress);
			out.write(byteTopicId);
			out.write(byteMessageType);
			out.write(byteDuplicate);
			out.write(byteWill);
			out.write(byteClean);
			out.write(byteRetain);
			out.write(byteReturnValue);
			out.write(byteLength);
			if (message.getLength() > 0) {
				out.write(message.getMessage());
			}
			
			// Flush and close
			out.flush();
			out.close();
			System.out.println("LOG: message sent!"); // DEBUG
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
			System.err.println("ERROR: exception while sending message!");
		} catch (IOException e) {
			e.printStackTrace();
			System.err.println("ERROR: exception while sending message!");
		}
	}
	
	/**
	 * SEND MESSAGES - Reverse a byte array (for endianness correction)
	 */
	public byte[] reverseBytes(byte[] array) {
		byte swapByte;
		int n = array.length;
		for (int i = 0; i < (n / 2); i++) {
			swapByte = array[i];
			array[i] = array[n - i - 1];
			array[n - i - 1] = swapByte;
		}
		return array;
	}
	
}
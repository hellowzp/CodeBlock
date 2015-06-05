package levocon.controller;

import java.io.IOException;

public class NetworkKiller {

	public void attachKiller(final Runtime runtime) {
		Runtime.getRuntime().addShutdownHook(new Thread() {
			@Override
			public void run() {
				// TODO Write proper shutdown sequence
				try {
					runtime.exec("kill -9 $(pgrep -f zigbee_network)");
				} catch (IOException e) {
					e.printStackTrace();
					System.err.println("ERROR: exception while killing network!");
				}
			}
		});
	}

}
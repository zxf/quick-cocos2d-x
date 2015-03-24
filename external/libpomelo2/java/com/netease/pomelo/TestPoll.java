/**
 * Copyright (c) 2014 NetEase, Inc. and other Pomelo contributors
 * MIT Licensed.
 */

package com.netease.pomelo;

import com.netease.pomelo.Client;

public class TestPoll {
  static public void main(String [] args) {

    Client.libInit(Client.PC_LOG_WARN, null, null);

    Client c = new Client();

    // tls, polling
    c.init(false, true, new Client.LocalStorage() {
      public int write(String s) {return -1; }
      public String read() {return null;}
    });

    c.connect("127.0.0.1", 3010);

    int handler_id = c.addEventHandler(new Client.EventHandler() {
        public void handle(int ev_type, String arg1, String arg2) {
            System.out.println("get push message: " + arg2);
            System.out.println("ev type: " + Client.evToStr(ev_type));
        }
    });

    try {
        Thread.sleep(1000);
    } catch (Exception e) {

    }
 
    c.request("connector.entryHandler.entry", "{\"name\": \"test\"}", 10, new Client.RequestCallback() {
        public void handle(int rc, String resp) {
            System.out.println("get resp:" + resp);
        }
    });

    c.notify("test.testHandler.notify", "{\"content\": \"test\"}", 10, new Client.NotifyCallback() {
        public void handle(int rc) {
            System.out.println("get status:" + Client.rcToStr(rc));
        }
    });

    try {
        Thread.sleep(10000);
    } catch (Exception e) {

    }

    c.poll();

    try {
        Thread.sleep(10000);
    } catch (Exception e) {

    }

    c.rmEventHandler(handler_id);

    c.destroy();
    Client.libCleanup();
  }
}

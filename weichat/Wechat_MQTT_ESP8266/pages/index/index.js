
//index.js
import mqtt from './../../utils/mqtt.js';

//获取应用实例
const app = getApp();

Page({
  data: {
    client: null,
    topic: {
      LEDcontrolTopic: 'sensor/work',
    },
    
  },

  onLoad: function() {
    var that = this;
    this.data.client = app.globalData.client;

    // console.log("on load");

    that.data.client.on('connect', that.ConnectCallback);
    that.data.client.on("message", that.MessageProcess);
    that.data.client.on("error", that.ConnectError);
    that.data.client.on("reconnect", that.ClientReconnect);
    that.data.client.on("offline", that.ClientOffline);
  },

  onShow: function() {
    // console.log("on show");
  },

  onHide: function() {
    console.log("on hide");
  },

  onUnload: function() {
    console.log("on unload");
    var that = this;
    that.data.client.end();
  },
  talks: function (e) {
    this.setData({
      talks: e.detail.value
    })
  },
  
  
  LedControl: function(e) {
    var that = this;
    console.log(this.data.talks)
    that.data.client.publish(that.data.topic.LEDcontrolTopic, this.data.talks, {
      qos: 1
    });
  },

  MessageProcess: function(topic, payload) {
    var that = this;

    var payload_string = payload.toString();
      

  },

  

  ConnectCallback: function(connack) {
    var that = this;

    for (var v in that.data.topic) {
      that.data.client.subscribe(that.data.topic[v], {
        qos: 1
      });
    }
  },

  ConnectError: function(error) {
    console.log(error)
  },

  ClientReconnect: function() {
    console.log("Client Reconnect")
  },

  ClientOffline: function() {
    console.log("Client Offline")
  }
   
})
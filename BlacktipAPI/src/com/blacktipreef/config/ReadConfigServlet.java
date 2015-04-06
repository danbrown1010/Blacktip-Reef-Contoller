package com.blacktipreef.config;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
//import java.util.Calendar;
import java.util.List;

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;


//@SuppressWarnings("serial")
public class ReadConfigServlet extends HttpServlet {
	public void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws IOException {
        String deviceid = req.getParameter("deviceId");
        try {
        	// Build the parameters 
        	List<NameValuePair> params = new ArrayList<NameValuePair>(); 
        	params.add(new BasicNameValuePair("tank1Lbl", "Display Tank")); 
        	params.add(new BasicNameValuePair("tempSensLb", "Temperature")); 
        	params.add(new BasicNameValuePair("tempUpLmt1", "80.00"));   
        	params.add(new BasicNameValuePair("tempLoLmt2", "77.00"));
        	params.add(new BasicNameValuePair("tempReadFreq", "1000")); 
        	params.add(new BasicNameValuePair("tempCldUpdt", "60000")); 
        	params.add(new BasicNameValuePair("phSensLbl", "PH"));   
        	params.add(new BasicNameValuePair("phUpLmt", "8.0"));   
        	params.add(new BasicNameValuePair("phLoLmt", "8.3"));
        	params.add(new BasicNameValuePair("phReadFreq", "1000")); 
        	params.add(new BasicNameValuePair("phCldUpdt", "60000")); 
        	params.add(new BasicNameValuePair("salSensLbl", "Salinity"));   
        	params.add(new BasicNameValuePair("salUpLmt", "1.025"));   
        	params.add(new BasicNameValuePair("salLoLmt", "1.023"));
        	params.add(new BasicNameValuePair("salReadFreq", "1000")); 
        	params.add(new BasicNameValuePair("salCldUpdt", "60000")); 
        	params.add(new BasicNameValuePair("orpSensLbl", "ORP"));   
        	params.add(new BasicNameValuePair("orpUpLmt", "450"));   
        	params.add(new BasicNameValuePair("orpLoLmt", "300"));
        	params.add(new BasicNameValuePair("orpReadFreq", "1000")); 
        	params.add(new BasicNameValuePair("orpCldUpdt", "60000")); 
        	params.add(new BasicNameValuePair("humSensLbl", "Room Humidity"));   
        	params.add(new BasicNameValuePair("humUpLmt", "20.00"));   
        	params.add(new BasicNameValuePair("humLoLmt", "70.00"));
        	params.add(new BasicNameValuePair("humReadFreq", "1000")); 
        	params.add(new BasicNameValuePair("humCldUpdt", "60000")); 
        	params.add(new BasicNameValuePair("relayLabel1", "Display Heater 1"));
        	params.add(new BasicNameValuePair("relayLabel2", "Display Heater 2"));
        	params.add(new BasicNameValuePair("relayLabel3", "Fan 1"));
        	params.add(new BasicNameValuePair("relayLabel4", "Fan 2"));
        	params.add(new BasicNameValuePair("relayLabel5", "None"));
        	params.add(new BasicNameValuePair("relayLabel6", "Quarantine Heater"));
        	params.add(new BasicNameValuePair("relayLabel7", "RO Heater"));
        	params.add(new BasicNameValuePair("relayLabel8", "Salt Heater"));        	
        	params.add(new BasicNameValuePair("cloudHost", "sixth-tempo-89216.appspot.com"));
        	params.add(new BasicNameValuePair("twilSid", "AC29fd98db5431536e202df77d77769575"));
        	params.add(new BasicNameValuePair("twilTok", "00a3b335a977d709380bdeaa53780da4"));
        	params.add(new BasicNameValuePair("twilNum","+12064294857"));
        	params.add(new BasicNameValuePair("txtNum", "2066126431"));        	
        	params.add(new BasicNameValuePair("vers", "0.2"));
        	params.add(new BasicNameValuePair("ErrCdReboot", "0"));
        	params.add(new BasicNameValuePair("ErrCdTemp", "1"));
        	params.add(new BasicNameValuePair("ErrCdPh", "2"));
        	params.add(new BasicNameValuePair("ErrCdORP", "3"));
        	params.add(new BasicNameValuePair("ErrCdHum", "4"));
        	params.add(new BasicNameValuePair("ErrCdSpill", "5"));
        	params.add(new BasicNameValuePair("ErrCdOther", "6"));
        	params.add(new BasicNameValuePair("ErrCdNetwork", "7"));        		 
        	try{ 
        		System.out.println("Sending Configs...");
            	PrintWriter writer = resp.getWriter();
            	//writer.write("$**\n");
            	for (int ii=0; ii < params.size(); ii++){
            		System.out.println("$"+params.get(ii).getName()+","+params.get(ii).getValue()+"*");
            		writer.write("$"+params.get(ii).getName()+","+params.get(ii).getValue()+"*");
                }
            	//writer.write("**$");
            	writer.close();
        	}catch (Exception e){
        		throw e;
        	}
        }catch (Exception e){
        	PrintWriter writer = resp.getWriter();
            writer.write("<html>\n");
            writer.write("NO CONFIGS SENT!");
            writer.write("<br>");
            writer.write("Error: "+e.toString());
            writer.write("<br>");            
            writer.write("</html>");
        }        
  	}
	
	
	
}

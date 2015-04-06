package com.blacktipreef.api;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Calendar;

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;

import java.util.*; 

import com.twilio.sdk.*; 
import com.twilio.sdk.resource.factory.*; 
import com.twilio.sdk.resource.instance.*; 
//import com.twilio.sdk.resource.list.*; 



@SuppressWarnings("serial")
public class SendTextServlet extends HttpServlet {
	
	// Find your Account Sid and Token at twilio.com/user/account 
	public static final String ACCOUNT_SID = "AC29fd98db5431536e202df77d77769575"; 
	public static final String AUTH_TOKEN = "00a3b335a977d709380bdeaa53780da4"; 
	public static final String TEXT_INTRO = "BRC v0.2 ALERT: "; 
	public String[] messages = {"Reboot", "High Temp", "Low Temp"};
	
	
	public void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws IOException {
        String codedstr = req.getParameter("body");
        String message = "";
        TwilioRestClient client = new TwilioRestClient(ACCOUNT_SID, AUTH_TOKEN); 
        try {
        	Integer ref = new Integer(codedstr);
        	// Build the parameters 
        	message = getMessageString(ref.intValue());
        	List<NameValuePair> params = new ArrayList<NameValuePair>(); 
        	params.add(new BasicNameValuePair("To", "2066126431")); 
        	params.add(new BasicNameValuePair("From", "+12064294857")); 
        	params.add(new BasicNameValuePair("Body", message));   
        		 
        	MessageFactory messageFactory = client.getAccount().getMessageFactory(); 
        	try{ 
        		Message messageFac = messageFactory.create(params); 
        		System.out.println(messageFac.getSid()+":"+message);
            	PrintWriter writer = resp.getWriter();
                writer.write("<html>\n");
                writer.write("TEXT SENT: "+messageFac.getSid()+":"+message);
                writer.write("<br>");
                writer.write(Calendar.getInstance().getTime().toString());
                writer.write("<br>");
                writer.write("</html>");
        	}catch (Exception e){
        		throw e;
        	}
        }catch (Exception e){
        	PrintWriter writer = resp.getWriter();
            writer.write("<html>\n");
            writer.write("NO TEXT WILL BE SENT: Invalid Message Code!");
            writer.write("<br>");
            writer.write("doGet(): " + codedstr +" "+ Calendar.getInstance().getTime().toString());
            writer.write("<br>");
            writer.write("Error: "+e.toString());
            writer.write("<br>");            
            writer.write("</html>");
        }        
  	}
	
	public void doPost(HttpServletRequest req, HttpServletResponse resp)
			throws IOException {
        String str = req.getParameter("body");
        PrintWriter writer = resp.getWriter();
        writer.write("<html>\n");
        writer.write("<b>");
        writer.write("doPost(): " + str + Calendar.getInstance().getTime().toString());
        writer.write("</b>");
        writer.write("</html>");
	}
	
	
	  public String getMessageString(int index)
	  {
	    return TEXT_INTRO+messages[index];
	  }

}

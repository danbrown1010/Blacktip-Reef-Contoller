package com.blacktipreef.api;


import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import com.google.appengine.api.users.User;
import com.google.appengine.api.users.UserService;
import com.google.appengine.api.users.UserServiceFactory;

import java.io.IOException;
import java.util.Date;



//import java.io.IOException;
import java.io.PrintWriter;
import java.util.Calendar;

import javax.servlet.http.*;

@SuppressWarnings("serial")
public class TankMeasurementServlet extends HttpServlet {
	public void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws IOException {
        String tankId = req.getParameter("tankId");
        
        UserService userService = UserServiceFactory.getUserService();
        User user = userService.getCurrentUser();

        Key tankKey = KeyFactory.createKey("TankId", tankId);
        String measurementContent = req.getParameter("measurementContent");
        String measurementType = req.getParameter("measurementType");
        Date date = new Date();
        Entity tankMeasurement = new Entity("TankMeasurement", tankKey);
        if (user != null) {
          tankMeasurement.setProperty("author_id", user.getUserId());
          tankMeasurement.setProperty("author_email", user.getEmail());
        }
        tankMeasurement.setProperty("date", date);
        tankMeasurement.setProperty("tank_id", tankId);
        tankMeasurement.setProperty("measurementType", measurementType);
        tankMeasurement.setProperty("content", measurementContent);

        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        datastore.put(tankMeasurement);

        PrintWriter writer = resp.getWriter();
        writer.write("<html>\n");
        writer.write("<b>");
        writer.write("doGet(): " + tankId + ", "+measurementType+", "+measurementContent+" Time: " + Calendar.getInstance().getTime().toString());
        writer.write("</b>");
        writer.write("</html>");  
	}
	

}

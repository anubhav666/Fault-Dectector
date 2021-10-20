import cv2
import numpy as np
import sys
import paho.mqtt.client as mqtt

mqttBroker = "192.168.0.100"  # mqtt localhost Broker
client = mqtt.Client("Comp")
client.connect(mqttBroker)
topic = "Detection"
cap = cv2.VideoCapture(0)
i = 0

while True:
    ret, frame = cap.read()

    cnv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    if ret is False:
        sys.exit()
    else:
        lower, upper = np.array([137, 13, 174]), np.array([179, 255, 255])
        mask = cv2.inRange(cnv_frame, lower, upper)
        mask = cv2.erode(mask, None, iterations=2)
        mask = cv2.dilate(mask, None, iterations=2)
        img = cv2.bitwise_and(frame, frame, mask=mask)
        contours, h = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        if len(contours) > 0:
            cv2.imshow("After contour detection", mask)
            cnts = sorted(contours, key=cv2.contourArea, reverse=True)[:10]  # stores first 10 largest contour points
            c = max(cnts, key=cv2.contourArea)
            ((x, y), radius) = cv2.minEnclosingCircle(c)
            if radius > 100:
                i += 1
                # draw the circle and centroid on the frame,
                # then update the list of tracked points
                cv2.rectangle(frame, (int(x - radius), int(y - radius)), (int(x + radius), int(y + radius)),
                              (0, 255, 255), 2)
                cv2.circle(frame, (int(x), int(y)), 2, (0, 0, 255), -1)
                text = "({},{})".format(int(x), int(y))
                cv2.putText(frame, text, (int(x), int(y - 10)), cv2.FONT_HERSHEY_PLAIN, 1, (0, 0, 255), 2)
            # done till finding out of the largest contour
        else:
            i = 0
        cv2.imshow("Live Detection", frame)
        if i == 1:
            print("Publishing: Faulty to Topic: Detection")
            client.publish(topic, "Faulty")
        if cv2.waitKey(1) & 0xFF == ord('q'):
            print("Keyboard interrupt")
            break

cap.release()
cv2.destroyAllWindows()

import 'package:flutter/material.dart';
import 'package:flutter_svg/svg.dart';
import 'package:firebase_database/firebase_database.dart';

import '../../../widgets/custom_card.dart';

class SensorsDetailsCard extends StatefulWidget {
  const SensorsDetailsCard({super.key});

  @override
  _SensorsDetailsCardState createState() => _SensorsDetailsCardState();
}

class _SensorsDetailsCardState extends State<SensorsDetailsCard> {
  late DatabaseReference _databaseReference; // Reference to Firebase Database

  final List<Map<String, dynamic>> streamDataDetails = [
    {"icon": 'assets/svg/speed.svg', "value": "60 Km/h", "title": "Speed"},
    {"icon": 'assets/svg/acceleration.svg', "value": "20 M/s2", "title": "Acceleration"},
    {"icon": 'assets/svg/temperature.svg', "value": "90 °c", "title": "Temperature"},
    {"icon": 'assets/svg/humidity.svg', "value": "60 %", "title": "Humidity"},
  ];

  @override
  void initState() {
    super.initState();
    _databaseReference = FirebaseDatabase.instance.ref('SensorsData/01');
    _listenToFirebaseData();
  }

  // Function to listen for changes in Firebase data
  void _listenToFirebaseData() {
    _databaseReference.onValue.listen((event) {
      final data = event.snapshot.value;
      if (data != null && data is Map<String, dynamic>) {
        setState(() {
          streamDataDetails[0]["value"] = "${data['speed']} Km/h";
          streamDataDetails[1]["value"] = "${data['acceleration']} M/s2";
          streamDataDetails[2]["value"] = "${data['temperature']} °C";
          streamDataDetails[3]["value"] = "${data['humidity']} %";
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return GridView.builder(
      itemCount: streamDataDetails.length,
      shrinkWrap: true,
      physics: const ScrollPhysics(),
      gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
        crossAxisCount: 2, // Always show 2 columns for mobile
        crossAxisSpacing: 15,
        mainAxisSpacing: 12.0,
      ),
      itemBuilder: (context, i) {
        return CustomCard(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              SvgPicture.asset(streamDataDetails[i]["icon"]),
              Padding(
                padding: const EdgeInsets.only(top: 15, bottom: 4),
                child: Text(
                  streamDataDetails[i]["value"],
                  style: const TextStyle(
                    fontSize: 18,
                    color: Colors.white,
                    fontWeight: FontWeight.w600,
                  ),
                ),
              ),
              Text(
                streamDataDetails[i]["title"],
                style: const TextStyle(
                  fontSize: 13,
                  color: Colors.grey,
                  fontWeight: FontWeight.normal,
                ),
              ),
            ],
          ),
        );
      },
    );
  }
}

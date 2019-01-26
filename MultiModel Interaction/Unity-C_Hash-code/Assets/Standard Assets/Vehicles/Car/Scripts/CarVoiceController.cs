using UnityEngine;
using System.Collections;
using UnityEngine.Windows.Speech;
using System.Collections.Generic;
using System;
using System.Linq;
using UnityStandardAssets.CrossPlatformInput;
namespace UnityStandardAssets.Vehicles.Car
{
    [RequireComponent(typeof(CarController))]
    public class CarVoiceController : MonoBehaviour
    {

        public CarController m_Car;

        private readonly CarController v_Car;

        CarVoiceController()
        {
            if (v_Car == null) {
                v_Car = Instantiate<CarController>(m_Car);
            }
        }

        private void Start()
        {

            // get the car controller
              //v_Car = Instantiate<CarController>(m_Car);
            //v_Car = c_Car;
            Debug.Log("CarKeyMouseController Start ");
        }

        // Use this for initialization
        void OnEnable()
        {
            CarUserControl.OnAction += VoiceControl;
            Debug.Log("CarVoiceController OnEnable ");
        }

        // Update is called once per frame
        void OnDisable()
        {
            CarUserControl.OnAction -= VoiceControl;
            Debug.Log("CarVoiceController OnDisable ");
        }

        private void VoiceControl()
        {
            float h = CrossPlatformInputManager.GetAxis("Horizontal");
            float v = CrossPlatformInputManager.GetAxis("Vertical");
            //Debug.Log(" KeyAndMouseControl Control " +v);
            //if (v !=0) { GetComponent<Rigidbody>().velocity = transform.forward * 2500f * Time.deltaTime; }
            CarVoiceController abc = new CarVoiceController();
            abc.v_Car.Move(h, v, v, 0f);
            /*key.Add("Go", () =>
            {

                h = 0f;
                v = 1f;
                GetComponent<Rigidbody>().velocity = transform.forward * 2500f * Time.deltaTime;
            });
            key.Add("Back", () =>
            {


                h = 0f;
                v = -1f;
                GetComponent<Rigidbody>().velocity = transform.forward * -2500f * Time.deltaTime;
            });
            key.Add("Left", () =>
            {
                h = -100f;
                v = 0f;
                transform.Rotate(Vector3.up, -1000f * Time.deltaTime);
            });
            key.Add("Right", () =>
            {
                h = 100f;
                v = 0f;
                transform.Rotate(Vector3.down, -1000f * Time.deltaTime);
            });
            keywordRecognizer = new KeywordRecognizer(key.Keys.ToArray());
            keywordRecognizer.OnPhraseRecognized += (PhraseRecognizedEventArgs eventArgs) =>
            {
                Debug.Log(" Verticle " + v);
                Debug.Log(" Horozontal " + h);
                key[eventArgs.text].Invoke();
               // m_Car.Move(h, v, v, 0f);
            };

            keywordRecognizer.Start();*/

        }
    }
}
